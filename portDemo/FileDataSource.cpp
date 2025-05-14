#include "FileDataSource.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>

FileDataSource::FileDataSource(const std::string& folderPath)
	: _folderPath(folderPath) {
	LoadImages();

	// 打印文件夹路径和文件列表
	std::cout << "Folder path: " << _folderPath << std::endl;
	for (const auto& file : _imageFiles) {
		std::cout << "Image file: " << file << std::endl;
	}
}

cv::Mat FileDataSource::_FetchRawImage() {
	if (_imageFiles.empty()) {
		// 如果没有更多图像，返回空图像
		return cv::Mat();
	}

	// 获取下一个图像文件路径
	std::string filePath = _folderPath + "/" + _imageFiles.front();
	_imageFiles.erase(_imageFiles.begin());

	// 读取图像
	cv::Mat image = cv::imread(filePath);
	if (image.empty()) {
		std::cerr << "Failed to load image: " << filePath << std::endl;
	}

	return image;
}

void FileDataSource::LoadImages() {
	// 获取文件夹中的所有图像文件
	for (const auto& entry : std::experimental::filesystem::directory_iterator(_folderPath)) {
		if (entry.path().extension() == ".png") {
			_imageFiles.push_back(entry.path().filename().string());
		}
	}
}