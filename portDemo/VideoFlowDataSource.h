#pragma once

#include "DataSource.h"
#include <opencv2/opencv.hpp>
#include <atomic>
#include <mutex>
#include <chrono>
#include <string>

class VideoFlowDataSource : public DataSource {
public:
	// ���캯����������Ƶ��Ŀ��֡��
	explicit VideoFlowDataSource(double targetFps = 10.0);

	// ���õ������
	void EnableDebugOutput(bool enable) { _debugOutput = enable; }

	// ��ȡ�ⲿ�����ͼ���ṩ����ʾ�̺߳ͻ���
	void AddImage(const cv::Mat& image);

	// ��ȡ��ʾ�õ�ͼ��
	cv::Mat* GetDisplayImage();

protected:
	// �̳в�ʵ�� _FetchRawImage ����
	cv::Mat _FetchRawImage() override;

private:
	double _targetFps;  // Ŀ��֡��
	double _frameIntervalMs;  // ֡��������룩
	std::chrono::time_point<std::chrono::system_clock> _lastFrameTime;  // ��¼���һ�λ�ȡͼ���ʱ��
	std::queue<cv::Mat> _imageQueue;  // ���ڴ洢�����ͼ�����
	std::mutex _imageMutex;  // ����ͼ����еĻ�����
	bool _debugOutput{ false };  // �Ƿ����õ������
};
