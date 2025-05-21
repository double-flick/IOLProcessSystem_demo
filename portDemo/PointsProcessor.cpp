#include "PointsProcessor.h"
#include <iostream>

PointsProcessor::PointsProcessor() {}

void PointsProcessor::UpdatePoints(const PointsPacket& packet) {
	std::lock_guard<std::mutex> lock(_pointsMutex);
	_latestPoints = packet;
}

PointsPacket PointsProcessor::GetLatestPoints() {
	std::lock_guard<std::mutex> lock(_pointsMutex);
	return _latestPoints;
}

void PointsProcessor::DrawPointsOnDisplayImage(cv::Mat& displayImage) {
	std::lock_guard<std::mutex> lock(_pointsMutex);

	// 如果点集为空，输出提示并返回
	if (_latestPoints.points.empty()) {
		std::cout << "警告：当前没有关键点可绘制！" << std::endl;
		return;
	}

	// 定义颜色
	const cv::Scalar redColor(0, 0, 255); // 红色
	const cv::Scalar blueColor(255, 0, 0); // 蓝色
	const cv::Scalar greenColor(0, 255, 0); // 绿色
	const int radius = 5;
	const int thickness = 2; // 空心圆

	int pointCount = _latestPoints.points.size();
	int redPoints = 2;
	int bluePoints = 7;
	int greenPoints = 7;

	for (size_t i = 0; i < pointCount; ++i) {
		const auto& point = _latestPoints.points[i];
		cv::Scalar color;

		if (i < redPoints) {
			color = redColor; // 前两个点用红色
		}
		else if (i < redPoints + bluePoints) {
			color = blueColor; // 接下来7个点用蓝色
		}
		else {
			color = greenColor; // 最后7个点用绿色
		}

		cv::circle(displayImage, cv::Point(point.first, point.second), radius, color, thickness);
	}
}
