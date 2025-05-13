#include "FolderDataSource.h"
#include <chrono>
#include <thread>
#include <stdexcept>
#include<experimental/filesystem>


namespace fs = std::experimental::filesystem;

FolderDataSource::FolderDataSource(const std::string& folderPath,
	double fps,
	bool randomOrder)
	: _folderPath(folderPath), _fps(fps), _randomOrder(randomOrder), _rng(std::random_device{}()), _currentIndex(0) {

	if (!fs::exists(folderPath)) {
		throw std::runtime_error("Folder does not exist: " + folderPath);
	}

	_LoadImagePaths();
	_lastFrameTime = std::chrono::system_clock::now();
}

void FolderDataSource::SetPlaybackSpeed(double fps) {
	_fps = fps;
}

void FolderDataSource::SetPlaybackSpeed(double fps) {
	_fps = fps;
}

void FolderDataSource::_LoadImagePaths() {
	_imagePaths.clear();

	for (const auto& entry : fs::directory_iterator(_folderPath)) {
		if (fs::is_regular_file(entry.status())) {
			std::string ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext == ".jpg" || ext == ".png" || ext == ".bmp") {
				_imagePaths.push_back(entry.path());
			}
		}
	}


	if (_imagePaths.empty()) {
		throw std::runtime_error("No valid images found in folder");
	}

	if (_randomOrder) {
		std::shuffle(_imagePaths.begin(), _imagePaths.end(), _rng);
	}
}

fs::path FolderDataSource::_GetNextImagePath() {
	if (_imagePaths.empty()) {
		_LoadImagePaths();  // 重新加载（循环播放）
	}

	fs::path nextPath = _imagePaths[_currentIndex];
	_currentIndex = (_currentIndex + 1) % _imagePaths.size();
	return nextPath;
}

cv::Mat FolderDataSource::_FetchRawImage() {
	// 控制帧率
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		now - _lastFrameTime);

	int targetDelay = static_cast<int>(1000.0 / _fps);
	if (elapsed.count() < targetDelay) {
		std::this_thread::sleep_for(
			std::chrono::milliseconds(targetDelay - elapsed.count()));
	}
	_lastFrameTime = std::chrono::system_clock::now();

	// 读取下一张图片
	fs::path imagePath = _GetNextImagePath();
	cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);

	if (image.empty()) {
		throw std::runtime_error("Failed to read image: " + imagePath.string());
	}

	return image;
}