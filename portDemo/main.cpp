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

// 控制程序的运行状态
std::atomic<bool> g_running{ true };

// 模拟接收关键点数据包的回调函数
void OnPointsPacketReceived(const PointsPacket& packet) {
	std::cout << "Received PointsPacket for image ID: " << packet.imageId << std::endl;
	for (const auto& point : packet.points) {
		std::cout << "Point: (" << point.first << ", " << point.second << ")" << std::endl;
	}
}

// 控制台输入线程，按 'q' 停止程序
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
	// 创建 VideoFlowDataSource 实例，用于模拟视频流
	VideoFlowDataSource videoFlowDataSource(10.0); // 设置目标 FPS 为 10（实际使用时与目标帧率相关）
	videoFlowDataSource.EnableDebugOutput(true); // 启用调试输出

	 // 创建视频流模拟器，模拟从 60Hz 的 MP4 文件读取视频
	VideoStreamSimulator videoSimulator("G:/AS-OCT/data/IOL/IOLvideos/2021-06-11_12-52-38__OCT.MP4", 60.0); // 设置目标帧率为 30Hz

	// 启动视频流模拟器，开始模拟视频流的输入
	videoSimulator.StartSimulation(videoFlowDataSource);

	// 创建网络管理器，连接到服务器
	NetworkManager networkManager;
	if (!networkManager.Connect("172.27.246.211", 9999)) {
		std::cerr << "Failed to connect to the server" << std::endl;
		return -1;
	}

	// 创建协议发送器和接收器
	ProtocolSender protocolSender(networkManager);
	ProtocolReceiver protocolReceiver(networkManager);

	// 关键点处理器
	PointsProcessor pointsProcessor;

	// 设置回调函数来处理接收到的关键点数据包
	protocolReceiver.SetMessageCallback([&pointsProcessor](const PointsPacket& packet) {
		pointsProcessor.UpdatePoints(packet);
	});

	// 启动控制台输入线程
	std::thread inputThread(ConsoleInputThread);

	// 主处理循环
	while (g_running) {
		// 从 VideoFlowDataSource 获取要显示的图像
		cv::Mat* displayImage = videoFlowDataSource.GetDisplayImage();
		if (displayImage != nullptr) {
			// 获取最新的关键点并绘制
			pointsProcessor.DrawPointsOnDisplayImage(*displayImage);

			// 显示图像
			cv::imshow("Video Display", *displayImage);
			cv::waitKey(1); // 显示图像
		}
		// 从缓存中抽取图像并发送到服务端处理
		videoFlowDataSource.FetchRawImageToCache();
		std::pair<cv::Mat, std::string> imagePair = videoFlowDataSource.GetNextImage();
		if (!imagePair.first.empty()) {
			// 编码并发送图像数据
			ImagePacket packet = ImageProcessor::EncodeImage(imagePair.first, imagePair.second);
			protocolSender.Send(packet);
		}
		else {
			std::cout << "Waiting for video frames..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 控制 CPU 占用
	}



	networkManager.Disconnect();

	return 0;
}