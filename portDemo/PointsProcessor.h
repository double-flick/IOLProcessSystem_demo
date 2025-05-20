#pragma once

#include "DataSource.h"
#include "PointsPacket.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <chrono> // ���ʱ������ͷ�ļ�

class PointsProcessor {
public:
	PointsProcessor(DataSource& dataSource, const std::string& outputFolder);

	void ProcessPointsPacket(const PointsPacket& packet);

private:
	DataSource& _dataSource;
	std::string _outputFolder; // ���Ŀ¼·��

	void DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image);
	std::string GenerateOutputPath(const std::string& imageId) const;

	// �������������㲢�����ʱ
	void LogProcessingTime(const std::string& imageId);
};