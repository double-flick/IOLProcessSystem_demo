#include "ImageProcessor.h"
#include <chrono>
#include <random>
#include <sstream>

ImagePacket ImageProcessor::EncodeImage(const cv::Mat& image, const std::string& id) {
	ImagePacket packet;

	// 编码为JPEG（可调整质量参数）
	cv::imencode(".jpg", image, packet.encodedData, { cv::IMWRITE_JPEG_QUALITY, 90 });

	// 记录图像信息
	packet.width = image.cols;
	packet.height = image.rows;
	packet.imageId = id;

	return packet;
}

cv::Mat ImageProcessor::DecodeImage(const std::vector<uchar>& data) {
	return cv::imdecode(data, cv::IMREAD_COLOR);
}
