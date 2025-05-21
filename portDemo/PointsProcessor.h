#pragma once

#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <mutex>

class PointsProcessor {
public:
	// ���캯��������Ҫ DataSource ����
	PointsProcessor();

	// ���¹ؼ��㼯
	void UpdatePoints(const PointsPacket& packet);

	// ��ȡ���µĹؼ��㼯
	PointsPacket GetLatestPoints();

	// ���ؼ�����Ƶ�ͼ����
	void DrawPointsOnDisplayImage(cv::Mat& displayImage);

private:
	PointsPacket _latestPoints;  // ���µĹؼ��㼯
	std::mutex _pointsMutex;  // �����ؼ��㼯�Ļ�����
};
