#include "FileDataSource.h"
#include "ImageProcessor.h"
#include "NetworkManager.h"
#include "ProtocolSender.h"
#include "ImagePacket.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	// 设置图像文件夹路径
	std::string folderPath = "E:/WORK/mvs/projects/senderDemo/portDemo/imagetest";

	// 创建数据源
	FileDataSource dataSource(folderPath);

	// 创建网络管理器
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// 创建协议发送器
	ProtocolSender protocolSender(networkManager);

	// 传输文件夹中的所有图像
	while (true) {
		std::pair<cv::Mat, std::string> imagePair = dataSource.GetNextImage();
		if (imagePair.first.empty()) {
			break; // 如果没有更多图像，退出循环
		}

		// 编码图像
		ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);

		// 发送图像
		protocolSender.Send(packet);

		// 等待一段时间再发送下一张图像
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// 断开连接
	networkManager.Disconnect();

	return 0;
}