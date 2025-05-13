#pragma once
// DataSource.h
#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <mutex>

class DataSource {
public:
	virtual ~DataSource() = default;

	// 主接口：获取图像+ID（自动缓存）
	std::pair<cv::Mat, std::string> GetNextImage() {
		cv::Mat rawImage = _FetchRawImage();
		std::string id = _GenerateId();

		std::lock_guard<std::mutex> lock(_cacheMutex);
		_imageCache[id] = rawImage.clone();

		return{ rawImage, id };
	}

	// 从缓存获取原图
	bool GetOriginalImage(const std::string& id, cv::Mat& outImage) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		auto it = _imageCache.find(id);
		if (it != _imageCache.end()) {
			outImage = it->second.clone();
			return true;
		}
		return false;
	}

	// 释放缓存
	void ReleaseImage(const std::string& id) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		_imageCache.erase(id);
	}

protected:
	// 子类必须实现的原始数据获取方法
	virtual cv::Mat _FetchRawImage() = 0;

private:
	std::unordered_map<std::string, cv::Mat> _imageCache;
	std::mutex _cacheMutex;

	std::string _GenerateId() {
		return std::to_string(
			std::chrono::high_resolution_clock::now()
			.time_since_epoch().count()
		);
	}
};