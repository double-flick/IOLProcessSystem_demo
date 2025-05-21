#include "VideoFlowDataSource.h"
#include "VideoStreamSimulator.h"
#include "ImageProcessor.h"
#include "NetworkManager.h"
#include "ProtocolSender.h"
#include "ProtocolReceiver.h"
#include "PointsPacket.h"
#include "PointsProcessor.h"
#include "VideoDataSource.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

// ���Ƴ��������״̬
std::atomic<bool> g_running{ true };

// ģ����չؼ������ݰ��Ļص�����
void OnPointsPacketReceived(const PointsPacket& packet) {
	std::cout << "Received PointsPacket for image ID: " << packet.imageId << std::endl;
	for (const auto& point : packet.points) {
		std::cout << "Point: (" << point.first << ", " << point.second << ")" << std::endl;
	}
}

// ����̨�����̣߳��� 'q' ֹͣ����
void ConsoleInputThread() {
	std::cout << "Press 'q' and Enter to stop the program..." << std::endl;
	while (g_running) {
		char input;
		std::cin >> input;
		if (input == 'q' || input == 'Q') {
			g_running = false;
			break;
		}
	}
}

int main() {
	// ���� VideoFlowDataSource ʵ��������ģ����Ƶ��
	VideoFlowDataSource videoFlowDataSource(10.0); // ����Ŀ�� FPS Ϊ 10��ʵ��ʹ��ʱ��Ŀ��֡����أ�
	videoFlowDataSource.EnableDebugOutput(true); // ���õ������

	 // ������Ƶ��ģ������ģ��� 60Hz �� MP4 �ļ���ȡ��Ƶ
	VideoStreamSimulator videoSimulator("G:/AS-OCT/data/IOL/IOLvideos/2021-06-11_12-52-38__OCT.MP4", 60.0); // ����Ŀ��֡��Ϊ 30Hz

	// ������Ƶ��ģ��������ʼģ����Ƶ��������
	videoSimulator.StartSimulation(videoFlowDataSource);

	// ������������������ӵ�������
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// ����Э�鷢�����ͽ�����
	ProtocolSender protocolSender(networkManager);
	ProtocolReceiver protocolReceiver(networkManager);

	// �ؼ��㴦����
	PointsProcessor pointsProcessor;

	// ���ûص�������������յ��Ĺؼ������ݰ�
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.UpdatePoints(packet);
	});

	// ��������̨�����߳�
	std::thread inputThread(ConsoleInputThread);

	// ������ѭ��
	while (g_running) {
		// �� VideoFlowDataSource ��ȡҪ��ʾ��ͼ��
		cv::Mat* displayImage = videoFlowDataSource.GetDisplayImage();
		if (displayImage != nullptr) {
			// ��ȡ���µĹؼ��㲢����
			pointsProcessor.DrawPointsOnDisplayImage(*displayImage);

			// ��ʾͼ��
			cv::imshow("Video Display", *displayImage);
			cv::waitKey(1); // ��ʾͼ��
		}
		// �ӻ����г�ȡͼ�񲢷��͵�����˴���
		videoFlowDataSource.FetchRawImageToCache();
		std::pair<cv::Mat, std::string> imagePair = videoFlowDataSource.GetNextImage();
		if (!imagePair.first.empty()) {
			// ���벢����ͼ������
			ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);
			protocolSender.Send(packet);
		}
		else {
			std::cout << "Waiting for video frames..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // ���� CPU ռ��
	}



	networkManager.Disconnect();

	return 0;
}