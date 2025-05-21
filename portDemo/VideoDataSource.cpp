#include "VideoDataSource.h"
#include <iostream>
#include <thread>
#include <sstream>

VideoDataSource::VideoDataSource(const std::string& videoPath, double targetFps)
	: _videoPath(videoPath), _targetFps(targetFps) {
	_InitializeVideoCapture();
	_frameIntervalMs = 1000.0 / _targetFps;
	_lastFrameTime = std::chrono::system_clock::now();
}

VideoDataSource::~VideoDataSource() {
	std::lock_guard<std::mutex> lock(_videoMutex);
	if (_videoCapture.isOpened()) {
		_videoCapture.release();
	}
	if (_debugOutput) {
		std::cout << _GetCurrentTimeString() << " [DEBUG] Video capture released." << std::endl;
	}
}

std::string VideoDataSource::_GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    
    std::tm bt;
    localtime_s(&bt, &timer);  // 使用安全的 localtime_s

    std::ostringstream oss;
    oss << std::put_time(&bt, "%H:%M:%S") << '.' << std::setfill('0') 
        << std::setw(3) << ms.count();
    return oss.str();
}

void VideoDataSource::_InitializeVideoCapture() {
	std::lock_guard<std::mutex> lock(_videoMutex);

	_videoCapture.open(_videoPath);
	if (!_videoCapture.isOpened()) {
		std::cerr << _GetCurrentTimeString() << " [ERROR] Could not open video file: "
			<< _videoPath << std::endl;
		_videoEnded = true;
		return;
	}

	if (_debugOutput) {
		std::cout << _GetCurrentTimeString() << " [DEBUG] Video opened successfully. "
			<< "Target FPS: " << _targetFps
			<< " (Interval: " << _frameIntervalMs << "ms)" << std::endl;
	}
}

cv::Mat VideoDataSource::_FetchRawImage() {
	if (_videoEnded) {
		if (_debugOutput) {
			std::cout << _GetCurrentTimeString() << " [DEBUG] Video ended, no more frames." << std::endl;
		}
		return cv::Mat();
	}

	std::lock_guard<std::mutex> lock(_videoMutex);

	if (!_videoCapture.isOpened()) {
		_videoEnded = true;
		return cv::Mat();
	}

	cv::Mat frame;
	if (_GetNextFrame(frame)) {
		if (_debugOutput) {
			std::cout << _GetCurrentTimeString() << " [DEBUG] Frame #" << _frameCounter
				<< " sent to buffer. Size: " << frame.cols << "x" << frame.rows
				<< std::endl;
		}
		_frameCounter++;
		return frame;
	}

	if (_debugOutput) {
		std::cout << _GetCurrentTimeString() << " [DEBUG] No frame available at this time." << std::endl;
	}
	return cv::Mat();
}

bool VideoDataSource::_GetNextFrame(cv::Mat& frame) {
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		now - _lastFrameTime).count();

	if (_debugOutput) {
		std::cout << _GetCurrentTimeString() << " [DEBUG] Time since last frame: "
			<< elapsed << "ms (target: " << _frameIntervalMs << "ms)" << std::endl;
	}

	// 严格按时间间隔获取帧
	if (elapsed < _frameIntervalMs) {
		return false;
	}

	// 获取下一帧
	if (!_videoCapture.read(frame)) {
		_videoEnded = true;
		_videoCapture.release();
		if (_debugOutput) {
			std::cout << _GetCurrentTimeString() << " [DEBUG] Video playback finished." << std::endl;
		}
		return false;
	}

	_lastFrameTime = now;
	return !frame.empty();
}