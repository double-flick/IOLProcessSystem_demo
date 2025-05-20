#include "FileDataSource.h"
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

std::atomic<bool> g_running{ true };

void OnPointsPacketReceived(const PointsPacket& packet) {
	std::cout << "Received PointsPacket for image ID: " << packet.imageId << std::endl;
	for (const auto& point : packet.points) {
		std::cout << "Point: (" << point.first << ", " << point.second << ")" << std::endl;
	}
}

// ����̨��������߳�
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
	// ������Ƶ����Դ (10fps)
	VideoDataSource videoSource("G:/AS-OCT/data/IOL/IOLvideos/2021-11-01_12-41-57__OCT.MP4", 1);
	videoSource.EnableDebugOutput(true);
	videoSource.SetCacheCapacity(100);

	// �������������
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// ����Э�鷢�����ͽ�����
	ProtocolSender protocolSender(networkManager);
	ProtocolReceiver protocolReceiver(networkManager);

	// �������Ŀ¼
	std::string outputFolder = "E:/WORK/mvs/projects/senderDemo/portDemo/processed_images";
	PointsProcessor pointsProcessor(videoSource, outputFolder);

	// ������Ϣ�ص�����
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.ProcessPointsPacket(packet);
	});

	// ��������̨�����߳�
	std::thread inputThread(ConsoleInputThread);

	// ������ѭ��
	while (g_running) {
		// ���Ի�ȡͼ��
		videoSource.FetchRawImageToCache();
		std::pair<cv::Mat, std::string> imagePair = videoSource.GetNextImage();

		if (!imagePair.first.empty()) {
			// ���벢����ͼ��
			ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);
			protocolSender.Send(packet);
		}
		else {
			// ����Ϊ��ʱ�Ĵ���

			std::cout << "Waiting for video frames..." << std::endl;
			

			// �ȴ�һ��ʱ��������
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// ���С���ӳٷ�ֹCPUռ�ù���
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// ����
	inputThread.join();
	networkManager.Disconnect();

	return 0;
}