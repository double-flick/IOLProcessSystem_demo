#include "ImageProcessor.h"
#include <chrono>
#include <random>
#include <sstream>

ImagePacket ImageProcessor::EncodeImage(const cv::Mat& image, const std::string& id) {
	ImagePacket packet;

	// 编码为JPEG（可调整质量参数）
	cv::imencode(".jpg", image, packet.data, { cv::IMWRITE_JPEG_QUALITY, 90 });

	// 设置数据长度
	packet.dataSize = packet.data.size(); // 设置数据长度为编码后数据的大小

	// 记录图像信息
	packet.imageId = id;

	return packet;
}

cv::Mat ImageProcessor::DecodeImage(const std::vector<uchar>& data) {
	return cv::imdecode(data, cv::IMREAD_COLOR);
}
