#include "VideoFlowDataSource.h"
#include <iostream>
#include <chrono>

// 构造函数，初始化目标帧率
VideoFlowDataSource::VideoFlowDataSource(double targetFps)
	: _targetFps(targetFps) {
	_frameIntervalMs = 1000.0 / targetFps;  // 计算每帧之间的时间间隔（毫秒）
	_lastFrameTime = std::chrono::system_clock::now();  // 初始化最后获取图像的时间
}

// 添加外部传入的图像到队列中
void VideoFlowDataSource::AddImage(const cv::Mat& image) {
	std::lock_guard<std::mutex> lock(_imageMutex);
	_imageQueue.push(image);  // 将图像加入队列
}

// 获取要显示的图像
cv::Mat* VideoFlowDataSource::GetDisplayImage() {
	std::lock_guard<std::mutex> lock(_imageMutex);
	if (!_imageQueue.empty()) {
		return &_imageQueue.front();  // 返回队列中的图像指针
	}
	return nullptr;  // 如果队列为空，返回空指针
}

// 重写 _FetchRawImage 方法，从队列中获取图像并按速率控制获取的时间
cv::Mat VideoFlowDataSource::_FetchRawImage() {
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastFrameTime).count();

	// 如果时间间隔小于目标帧率的间隔，返回空图像，避免过快取图
	if (elapsed < _frameIntervalMs) {
		return cv::Mat();  // 如果时间间隔太短，直接返回空图像
	}

	// 获取图像并更新上次获取图像的时间
	std::lock_guard<std::mutex> lock(_imageMutex);
	if (!_imageQueue.empty()) {
		cv::Mat image = _imageQueue.front();  // 获取队列中的图像
		_imageQueue.pop();  // 从队列中移除该图像
		_lastFrameTime = now;  // 更新最后获取图像的时间
		return image;
	}

	return cv::Mat();  // 如果队列为空，返回空图像
}
