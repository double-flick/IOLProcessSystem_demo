#include "FileDataSource.h"
#include "ImageProcessor.h"
#include "NetworkManager.h"
#include "ProtocolSender.h"
#include "ImagePacket.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	// ����ͼ���ļ���·��
	std::string folderPath = "E:/WORK/mvs/projects/senderDemo/portDemo/imagetest";

	// ��������Դ
	FileDataSource dataSource(folderPath);

	// �������������
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// ����Э�鷢����
	ProtocolSender protocolSender(networkManager);

	// �����ļ����е�����ͼ��
	while (true) {
		std::pair<cv::Mat, std::string> imagePair = dataSource.GetNextImage();
		if (imagePair.first.empty()) {
			break; // ���û�и���ͼ���˳�ѭ��
		}

		// ����ͼ��
		ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);

		// ����ͼ��
		protocolSender.Send(packet);

		// �ȴ�һ��ʱ���ٷ�����һ��ͼ��
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// �Ͽ�����
	networkManager.Disconnect();

	return 0;
}