#include "VideoStreamSimulator.h"
#include <iostream>
#include <chrono>

// 构造函数，初始化视频文件路径和目标帧率
VideoStreamSimulator::VideoStreamSimulator(const std::string& videoPath, double targetFps)
	: _videoPath(videoPath), _targetFps(targetFps), _running(false) {}

// 启动模拟器，开始读取视频文件并提供图像
void VideoStreamSimulator::StartSimulation(VideoFlowDataSource& videoFlowDataSource) {
	_running = true;
	_simulationThread = std::thread(&VideoStreamSimulator::ReadAndFeedFrames, this, std::ref(videoFlowDataSource));
}

// 停止模拟器
void VideoStreamSimulator::StopSimulation() {
	_running = false;
	if (_simulationThread.joinable()) {
		_simulationThread.join(); // 等待线程结束
	}
}

// 从视频文件中读取帧并将其提供给 VideoFlowDataSource
void VideoStreamSimulator::ReadAndFeedFrames(VideoFlowDataSource& videoFlowDataSource) {
	cv::VideoCapture videoCapture(_videoPath);
	if (!videoCapture.isOpened()) {
		std::cerr << "Error: Could not open video file: " << _videoPath << std::endl;
		return;
	}

	double fps = videoCapture.get(cv::CAP_PROP_FPS);  // 获取视频的实际帧率（60Hz）
	std::cout << "Video FPS: " << fps << std::endl;

	cv::Mat frame;
	int frameCount = 0;

	while (_running) {
		// 读取一帧
		if (!videoCapture.read(frame)) {
			std::cerr << "Error: Could not read frame from video file." << std::endl;
			break;
		}

		// 提供图像给 VideoFlowDataSource，每秒 30 帧（30Hz）
		if (frameCount % (int)(fps / _targetFps) == 0) {
			videoFlowDataSource.AddImage(frame);  // 将图像添加到 VideoFlowDataSource
		}

		frameCount++;

		// 控制读取速率，保证 30Hz 提供图像
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / _targetFps)));
	}

	videoCapture.release();
}
