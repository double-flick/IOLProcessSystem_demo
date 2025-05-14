// PointsProcessor.h
#pragma once
#include "DataSource.h"
#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>

class PointsProcessor {
public:
	// 构造函数增加保存路径参数
	PointsProcessor(DataSource& dataSource, const std::string& outputFolder);

	void ProcessPointsPacket(const PointsPacket& packet);

private:
	DataSource& _dataSource;
	std::string _outputFolder; // 图像保存路径

	void DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image);
	std::string GenerateOutputPath(const std::string& imageId) const;
};