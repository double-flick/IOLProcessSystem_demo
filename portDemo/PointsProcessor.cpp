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

	// ����㼯Ϊ�գ������ʾ������
	if (_latestPoints.points.empty()) {
		std::cout << "���棺��ǰû�йؼ���ɻ��ƣ�" << std::endl;
		return;
	}

	// ������ɫ
	const cv::Scalar redColor(0, 0, 255); // ��ɫ
	const cv::Scalar blueColor(255, 0, 0); // ��ɫ
	const cv::Scalar greenColor(0, 255, 0); // ��ɫ
	const int radius = 5;
	const int thickness = 2; // ����Բ

	int pointCount = _latestPoints.points.size();
	int redPoints = 2;
	int bluePoints = 7;
	int greenPoints = 7;

	for (size_t i = 0; i < pointCount; ++i) {
		const auto& point = _latestPoints.points[i];
		cv::Scalar color;

		if (i < redPoints) {
			color = redColor; // ǰ�������ú�ɫ
		}
		else if (i < redPoints + bluePoints) {
			color = blueColor; // ������7��������ɫ
		}
		else {
			color = greenColor; // ���7��������ɫ
		}

		cv::circle(displayImage, cv::Point(point.first, point.second), radius, color, thickness);
	}
}
