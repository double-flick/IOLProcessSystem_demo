#pragma once

#include "DataSource.h"
#include <opencv2/opencv.hpp>
#include <atomic>
#include <mutex>
#include <chrono>
#include <string>

class VideoFlowDataSource : public DataSource {
public:
	// 构造函数，传入视频的目标帧率
	explicit VideoFlowDataSource(double targetFps = 10.0);

	// 启用调试输出
	void EnableDebugOutput(bool enable) { _debugOutput = enable; }

	// 获取外部输入的图像，提供给显示线程和缓存
	void AddImage(const cv::Mat& image);

	// 获取显示用的图像
	cv::Mat* GetDisplayImage();

protected:
	// 继承并实现 _FetchRawImage 方法
	cv::Mat _FetchRawImage() override;

private:
	double _targetFps;  // 目标帧率
	double _frameIntervalMs;  // 帧间隔（毫秒）
	std::chrono::time_point<std::chrono::system_clock> _lastFrameTime;  // 记录最后一次获取图像的时间
	std::queue<cv::Mat> _imageQueue;  // 用于存储传入的图像队列
	std::mutex _imageMutex;  // 保护图像队列的互斥锁
	bool _debugOutput{ false };  // 是否启用调试输出
};
