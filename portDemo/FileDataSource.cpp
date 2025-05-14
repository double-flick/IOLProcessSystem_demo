#include "FileDataSource.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>

FileDataSource::FileDataSource(const std::string& folderPath)
	: _folderPath(folderPath) {
	LoadImages();

	// ��ӡ�ļ���·�����ļ��б�
	std::cout << "Folder path: " << _folderPath << std::endl;
	for (const auto& file : _imageFiles) {
		std::cout << "Image file: " << file << std::endl;
	}
}

cv::Mat FileDataSource::_FetchRawImage() {
	if (_imageFiles.empty()) {
		// ���û�и���ͼ�񣬷��ؿ�ͼ��
		return cv::Mat();
	}

	// ��ȡ��һ��ͼ���ļ�·��
	std::string filePath = _folderPath + "/" + _imageFiles.front();
	_imageFiles.erase(_imageFiles.begin());

	// ��ȡͼ��
	cv::Mat image = cv::imread(filePath);
	if (image.empty()) {
		std::cerr << "Failed to load image: " << filePath << std::endl;
	}

	return image;
}

void FileDataSource::LoadImages() {
	// ��ȡ�ļ����е�����ͼ���ļ�
	for (const auto& entry : std::experimental::filesystem::directory_iterator(_folderPath)) {
		if (entry.path().extension() == ".png") {
			_imageFiles.push_back(entry.path().filename().string());
		}
	}
}