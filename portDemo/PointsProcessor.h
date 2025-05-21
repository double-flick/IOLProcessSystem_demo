#pragma once

#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <mutex>

class PointsProcessor {
public:
	// 构造函数不再需要 DataSource 参数
	PointsProcessor();

	// 更新关键点集
	void UpdatePoints(const PointsPacket& packet);

	// 获取最新的关键点集
	PointsPacket GetLatestPoints();

	// 将关键点绘制到图像上
	void DrawPointsOnDisplayImage(cv::Mat& displayImage);

private:
	PointsPacket _latestPoints;  // 最新的关键点集
	std::mutex _pointsMutex;  // 保护关键点集的互斥锁
};
