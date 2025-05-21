#include "VideoFlowDataSource.h"
#include <iostream>
#include <chrono>

// ���캯������ʼ��Ŀ��֡��
VideoFlowDataSource::VideoFlowDataSource(double targetFps)
	: _targetFps(targetFps) {
	_frameIntervalMs = 1000.0 / targetFps;  // ����ÿ֮֡���ʱ���������룩
	_lastFrameTime = std::chrono::system_clock::now();  // ��ʼ������ȡͼ���ʱ��
}

// ����ⲿ�����ͼ�񵽶�����
void VideoFlowDataSource::AddImage(const cv::Mat& image) {
	std::lock_guard<std::mutex> lock(_imageMutex);
	_imageQueue.push(image);  // ��ͼ��������
}

// ��ȡҪ��ʾ��ͼ��
cv::Mat* VideoFlowDataSource::GetDisplayImage() {
	std::lock_guard<std::mutex> lock(_imageMutex);
	if (!_imageQueue.empty()) {
		return &_imageQueue.front();  // ���ض����е�ͼ��ָ��
	}
	return nullptr;  // �������Ϊ�գ����ؿ�ָ��
}

// ��д _FetchRawImage �������Ӷ����л�ȡͼ�񲢰����ʿ��ƻ�ȡ��ʱ��
cv::Mat VideoFlowDataSource::_FetchRawImage() {
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastFrameTime).count();

	// ���ʱ����С��Ŀ��֡�ʵļ�������ؿ�ͼ�񣬱������ȡͼ
	if (elapsed < _frameIntervalMs) {
		return cv::Mat();  // ���ʱ����̫�̣�ֱ�ӷ��ؿ�ͼ��
	}

	// ��ȡͼ�񲢸����ϴλ�ȡͼ���ʱ��
	std::lock_guard<std::mutex> lock(_imageMutex);
	if (!_imageQueue.empty()) {
		cv::Mat image = _imageQueue.front();  // ��ȡ�����е�ͼ��
		_imageQueue.pop();  // �Ӷ������Ƴ���ͼ��
		_lastFrameTime = now;  // ��������ȡͼ���ʱ��
		return image;
	}

	return cv::Mat();  // �������Ϊ�գ����ؿ�ͼ��
}
