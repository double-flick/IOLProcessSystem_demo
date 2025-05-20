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

	// ���û�������
	void SetCacheCapacity(size_t capacity) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		_cacheCapacity = capacity;
	}

	// �ӻ����а�˳��ȡ�����ݣ����͵������
	std::pair<cv::Mat, std::string> GetNextImage() {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		if (_lruCache.empty()) {
			return{ cv::Mat(), "" }; // �������Ϊ�գ����ؿ�ͼ��Ϳ�ID
		}

		// ��ȡ�������ʹ�õĻ�����
		std::string id = _lruCache.ReplCandidate();
		cv::Mat image = _imageCache[id].clone();

		// ���Ƴ������Ϊ��������Ҫͨ�� GetOriginalImage ��ȡԭͼ
		return{ image, id };
	}

	// ������Դ�������ݵ�����
	void FetchRawImageToCache() {
		cv::Mat rawImage = _FetchRawImage();
		if (!rawImage.empty()) {
			AddImageToCache(rawImage);
		}
	}

	// ���� ID ��ȡ�����е����ݣ���ֱ�������ͼ��
	cv::Mat GetOriginalImage(const std::string& id) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		auto it = _imageCache.find(id);
		if (it == _imageCache.end()) {
			return cv::Mat(); // ���δ�ҵ������ؿ�ͼ��
		}

		cv::Mat image = it->second.clone();

		// ֱ�������ͼ��
		_imageCache.erase(it);
		_lruCache.Erase(id);

		return image;
	}

protected:
	virtual cv::Mat _FetchRawImage() = 0; // ������ʵ�֣����ڴ�����Դ��������

										  // �ṩ������ķ��������ڽ�ͼ����ӵ�����
	void AddImageToCache(const cv::Mat& image) {
		std::string id = _GenerateId();
		AddToCache(id, image);
	}

private:
	std::unordered_map<std::string, cv::Mat> _imageCache; // ʵ�ʴ洢ͼ��Ļ���
	LRUCachePolicy<std::string> _lruCache; // ʹ�� LRU �������
	std::mutex _cacheMutex; // ����������Դ�Ļ�����
	size_t _cacheCapacity = 100; // Ĭ�ϻ�������

								 // ����Ψһ ID
	std::string _GenerateId() {
		return std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	}

	// ��ͼ����뻺�棬��ִ���������
	void AddToCache(const std::string& id, const cv::Mat& image) {
		std::lock_guard<std::mutex> lock(_cacheMutex);
		_imageCache[id] = image.clone();
		_lruCache.Insert(id);
		//std::cout << "��ǰ�������: " << _imageCache.size() << std::endl;

		// ��黺���С�Ƿ񳬹���������
		while (_imageCache.size() > _cacheCapacity) {
			// ��ȡ���δʹ�õĻ�����
			std::string candidate = _lruCache.ReplCandidate();
			_imageCache.erase(candidate); // �ӻ������Ƴ�
			_lruCache.Erase(candidate); // �� LRU �б����Ƴ�
			std::cout << "��������������: " << candidate << std::endl;
		}
	}
};