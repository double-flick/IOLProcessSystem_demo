// FileDataSource.cpp
#include "FileDataSource.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <chrono>

namespace fs = std::experimental::filesystem;

FileDataSource::FileDataSource(const std::string& folderPath)
	: _folderPath(folderPath) {
	// 加载图像路径
	LoadImagePaths();

	// 启动预加载线程
	_preloadThread = std::thread(&FileDataSource::PreloadCacheThreadFunc, this);
}

FileDataSource::~FileDataSource() {
	// 停止预加载线程
	_stopPreload = true;

	// 等待线程结束
	if (_preloadThread.joinable()) {
		_preloadThread.join();
	}
}

void FileDataSource::LoadImagePaths() {
	try {
		for (const auto& entry : fs::directory_iterator(_folderPath)) {
			if (entry.path().extension() == ".png" ||
				entry.path().extension() == ".jpg") {
				std::lock_guard<std::mutex> lock(_fileListMutex);
				_pendingImageFiles.push_back(entry.path().string());
			}
		}
		std::cout << "Found " << _pendingImageFiles.size() << " images to load.\n";
	}
	catch (const fs::filesystem_error& e) {
		std::cerr << "Failed to load image paths: " << e.what() << std::endl;
	}
}


cv::Mat FileDataSource::_FetchRawImage() {
	std::string filePath = _GetNextFilePath();
	if (filePath.empty()) {
		return cv::Mat();
	}

	cv::Mat image = cv::imread(filePath);
	if (image.empty()) {
		std::cerr << "Failed to read image: " << filePath << std::endl;
	}
	
	return image;
}

std::string FileDataSource::_GetNextFilePath() {
	std::lock_guard<std::mutex> lock(_fileListMutex);
	if (_pendingImageFiles.empty()) {
		return "";
	}

	std::string filePath = _pendingImageFiles.back();
	_pendingImageFiles.pop_back();
	return filePath;
}

void FileDataSource::PreloadCacheThreadFunc() {
	while (!_stopPreload) {
		// 直接调用基类的方法，它会处理缓存逻辑
		// 等待一段时间再发送下一张图像
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		FetchRawImageToCache();
	}
}