#pragma once

#include "DataSource.h"
#include <opencv2/opencv.hpp>
#include <atomic>
#include <mutex>
#include <chrono>
#include <string>
#include <iomanip>

class VideoDataSource : public DataSource {
public:
	explicit VideoDataSource(const std::string& videoPath, double targetFps = 10.0);
	~VideoDataSource();

	VideoDataSource(const VideoDataSource&) = delete;
	VideoDataSource& operator=(const VideoDataSource&) = delete;

	void EnableDebugOutput(bool enable) { _debugOutput = enable; }

protected:
	cv::Mat _FetchRawImage() override;

private:
	void _InitializeVideoCapture();
	bool _GetNextFrame(cv::Mat& frame);
	std::string _GetCurrentTimeString();

	std::string _videoPath;
	cv::VideoCapture _videoCapture;
	double _targetFps;
	double _frameIntervalMs;
	std::chrono::time_point<std::chrono::system_clock> _lastFrameTime;
	std::mutex _videoMutex;
	std::atomic<bool> _videoEnded{ false };
	std::atomic<bool> _debugOutput{ false };
	int _frameCounter{ 0 };
};