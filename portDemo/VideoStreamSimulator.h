#pragma once

#include "VideoFlowDataSource.h"
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

class VideoStreamSimulator {
public:
	// 构造函数，传入视频文件路径和目标帧率
	explicit VideoStreamSimulator(const std::string& videoPath, double targetFps = 30.0);

	// 启动模拟器，开始读取视频文件并提供图像
	void StartSimulation(VideoFlowDataSource& videoFlowDataSource);

	// 停止模拟器
	void StopSimulation();

private:
	// 从视频文件中读取帧并将其提供给 VideoFlowDataSource
	void ReadAndFeedFrames(VideoFlowDataSource& videoFlowDataSource);

	std::string _videoPath;        // 视频文件路径
	double _targetFps;            // 目标帧率（例如 30Hz）
	std::atomic<bool> _running;    // 控制线程运行的标志
	std::thread _simulationThread; // 用于读取和提供图像的线程
};
