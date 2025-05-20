// DataSource.h
#pragma once

#include "cache_policy.hpp"
#include <lru_cache_policy.hpp>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <mutex>
#include <string>
#include <iostream>
#include <chrono>

using caches::LRUCachePolicy;

class DataSource {
public:
	virtual ~DataSource() = default;

	// 设置缓存容量
	void SetCacheCapacity(size_t capacity) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		_cacheCapacity = capacity;
	}

	// 从缓存中按顺序取出数据，发送到服务端
	std::pair<cv::Mat, std::string> GetNextImage() {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		if (_lruCache.empty()) {
			return{ cv::Mat(), "" }; // 如果缓存为空，返回空图像和空ID
		}

		// 获取最近最少使用的缓存项
		std::string id = _lruCache.ReplCandidate();
		cv::Mat image = _imageCache[id].clone();

		// 不移除该项，因为后续还需要通过 GetOriginalImage 获取原图
		return{ image, id };
	}

	// 从数据源加载数据到缓存
	void FetchRawImageToCache() {
		cv::Mat rawImage = _FetchRawImage();
		if (!rawImage.empty()) {
			AddImageToCache(rawImage);
		}
	}

	// 根据 ID 获取缓存中的数据，并直接驱逐该图像
	cv::Mat GetOriginalImage(const std::string& id) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		auto it = _imageCache.find(id);
		if (it == _imageCache.end()) {
			return cv::Mat(); // 如果未找到，返回空图像
		}

		cv::Mat image = it->second.clone();

		// 直接驱逐该图像
		_imageCache.erase(it);
		_lruCache.Erase(id);

		return image;
	}

protected:
	virtual cv::Mat _FetchRawImage() = 0; // 由子类实现，用于从数据源加载数据

										  // 提供给子类的方法，用于将图像添加到缓存
	void AddImageToCache(const cv::Mat& image) {
		std::string id = _GenerateId();
		AddToCache(id, image);
	}

private:
	std::unordered_map<std::string, cv::Mat> _imageCache; // 实际存储图像的缓存
	LRUCachePolicy<std::string> _lruCache; // 使用 LRU 缓存策略
	std::mutex _cacheMutex; // 保护共享资源的互斥锁
	size_t _cacheCapacity = 100; // 默认缓存容量

								 // 生成唯一 ID
	std::string _GenerateId() {
		return std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	}

	// 将图像加入缓存，并执行驱逐策略
	void AddToCache(const std::string& id, const cv::Mat& image) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		_imageCache[id] = image.clone();
		_lruCache.Insert(id);
		//std::cout << "当前缓存个数: " << _imageCache.size() << std::endl;

		// 检查缓存大小是否超过容量限制
		while (_imageCache.size() > _cacheCapacity) {
			// 获取最久未使用的缓存项
			std::string candidate = _lruCache.ReplCandidate();
			_imageCache.erase(candidate); // 从缓存中移除
			_lruCache.Erase(candidate); // 从 LRU 列表中移除
			std::cout << "缓存已满，驱逐: " << candidate << std::endl;
		}
	}
};