// PointsProcessor.h
#pragma once
#include "DataSource.h"
#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>

class PointsProcessor {
public:
	// ���캯�����ӱ���·������
	PointsProcessor(DataSource& dataSource, const std::string& outputFolder);

	void ProcessPointsPacket(const PointsPacket& packet);

private:
	DataSource& _dataSource;
	std::string _outputFolder; // ͼ�񱣴�·��

	void DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image);
	std::string GenerateOutputPath(const std::string& imageId) const;
};