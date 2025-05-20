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


// 测试用回调函数，用于处理接收到的 PointsPacket并打印坐标点
void OnPointsPacketReceived(const PointsPacket& packet) {
    std::cout << "Received PointsPacket for image ID: " << packet.imageId << std::endl;
    for (const auto& point : packet.points) {
        std::cout << "Point: (" << point.first << ", " << point.second << ")" << std::endl;
    }
}

int main() {
	// 设置图像文件夹路径
	std::string folderPath = "E:/WORK/mvs/projects/senderDemo/portDemo/imagetest2";

	// 创建数据源
	FileDataSource dataSource(folderPath);
	// 设置缓存容量
	dataSource.SetCacheCapacity(50); // 设置缓存容量为50


	// 创建网络管理器
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// 创建协议发送器
	ProtocolSender protocolSender(networkManager);

	// 创建协议接收器
	ProtocolReceiver protocolReceiver(networkManager);

	// 设置输出目录（可以改为您想要的路径）
	std::string outputFolder = "E:/WORK/mvs/projects/senderDemo/portDemo/processed_images";

	// 创建PointsProcessor实例
	PointsProcessor pointsProcessor(dataSource, outputFolder);

	// 设置消息回调函数
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.ProcessPointsPacket(packet);
	});

	// 传输文件夹中的所有图像
	while (true) {
		// 先填充缓存
		dataSource.FetchRawImageToCache();

		// 然后从缓存获取图片
		std::pair<cv::Mat, std::string> imagePair = dataSource.GetNextImage();
		if (imagePair.first.empty()) {
			break; // 如果没有更多图像，退出循环
		}

		// 编码图像
		ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);

		// 发送图像
		protocolSender.Send(packet);

	}

	// 等待接收线程结束
	std::this_thread::sleep_for(std::chrono::seconds(5));

	// 断开连接
	networkManager.Disconnect();

	return 0;
}