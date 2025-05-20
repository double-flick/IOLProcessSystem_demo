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

// 控制台输入监听线程
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
	// 创建视频数据源 (10fps)
	VideoDataSource videoSource("G:/AS-OCT/data/IOL/IOLvideos/2021-11-01_12-41-57__OCT.MP4", 1);
	videoSource.EnableDebugOutput(true);
	videoSource.SetCacheCapacity(100);

	// 创建网络管理器
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// 创建协议发送器和接收器
	ProtocolSender protocolSender(networkManager);
	ProtocolReceiver protocolReceiver(networkManager);

	// 设置输出目录
	std::string outputFolder = "E:/WORK/mvs/projects/senderDemo/portDemo/processed_images";
	PointsProcessor pointsProcessor(videoSource, outputFolder);

	// 设置消息回调函数
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.ProcessPointsPacket(packet);
	});

	// 启动控制台输入线程
	std::thread inputThread(ConsoleInputThread);

	// 主处理循环
	while (g_running) {
		// 尝试获取图像
		videoSource.FetchRawImageToCache();
		std::pair<cv::Mat, std::string> imagePair = videoSource.GetNextImage();

		if (!imagePair.first.empty()) {
			// 编码并发送图像
			ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);
			protocolSender.Send(packet);
		}
		else {
			// 缓存为空时的处理

			std::cout << "Waiting for video frames..." << std::endl;
			

			// 等待一段时间再重试
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// 添加小的延迟防止CPU占用过高
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// 清理
	inputThread.join();
	networkManager.Disconnect();

	return 0;
}