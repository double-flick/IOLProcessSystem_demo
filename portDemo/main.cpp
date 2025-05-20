#include "FileDataSource.h"
#include "ImageProcessor.h"
#include "NetworkManager.h"
#include "ProtocolSender.h"
#include "ProtocolReceiver.h"
#include "PointsPacket.h"
#include "PointsProcessor.h"
#include <iostream>
#include <thread>
#include <chrono>


// �����ûص����������ڴ�����յ��� PointsPacket����ӡ�����
void OnPointsPacketReceived(const PointsPacket& packet) {
    std::cout << "Received PointsPacket for image ID: " << packet.imageId << std::endl;
    for (const auto& point : packet.points) {
        std::cout << "Point: (" << point.first << ", " << point.second << ")" << std::endl;
    }
}

int main() {
	// ����ͼ���ļ���·��
	std::string folderPath = "E:/WORK/mvs/projects/senderDemo/portDemo/imagetest2";

	// ��������Դ
	FileDataSource dataSource(folderPath);
	// ���û�������
	dataSource.SetCacheCapacity(50); // ���û�������Ϊ50


	// �������������
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// ����Э�鷢����
	ProtocolSender protocolSender(networkManager);

	// ����Э�������
	ProtocolReceiver protocolReceiver(networkManager);

	// �������Ŀ¼�����Ը�Ϊ����Ҫ��·����
	std::string outputFolder = "E:/WORK/mvs/projects/senderDemo/portDemo/processed_images";

	// ����PointsProcessorʵ��
	PointsProcessor pointsProcessor(dataSource, outputFolder);

	// ������Ϣ�ص�����
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.ProcessPointsPacket(packet);
	});

	// �����ļ����е�����ͼ��
	while (true) {
		// ����仺��
		dataSource.FetchRawImageToCache();

		// Ȼ��ӻ����ȡͼƬ
		std::pair<cv::Mat, std::string> imagePair = dataSource.GetNextImage();
		if (imagePair.first.empty()) {
			break; // ���û�и���ͼ���˳�ѭ��
		}

		// ����ͼ��
		ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);

		// ����ͼ��
		protocolSender.Send(packet);

	}

	// �ȴ������߳̽���
	std::this_thread::sleep_for(std::chrono::seconds(5));

	// �Ͽ�����
	networkManager.Disconnect();

	return 0;
}