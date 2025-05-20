#pragma once

#include "DataSource.h"
#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <chrono> // 添加时间戳相关头文件

class PointsProcessor {
public:
	PointsProcessor(DataSource& dataSource, const std::string& outputFolder);

	void ProcessPointsPacket(const PointsPacket& packet);

private:
	DataSource& _dataSource;
	std::string _outputFolder; // 输出目录路径

	void DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image);
	std::string GenerateOutputPath(const std::string& imageId) const;

	// 新增方法：计算并输出耗时
	void LogProcessingTime(const std::string& imageId);
};