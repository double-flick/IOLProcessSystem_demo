#include "ImageProcessor.h"
#include <chrono>
#include <random>
#include <sstream>

ImagePacket ImageProcessor::EncodeImage(const cv::Mat& image, const std::string& id) {
	ImagePacket packet;

	// ����ΪJPEG���ɵ�������������
	cv::imencode(".jpg", image, packet.encodedData, { cv::IMWRITE_JPEG_QUALITY, 90 });

	// ��¼ͼ����Ϣ
	packet.width = image.cols;
	packet.height = image.rows;
	packet.imageId = id;

	return packet;
}

cv::Mat ImageProcessor::DecodeImage(const std::vector<uchar>& data) {
	return cv::imdecode(data, cv::IMREAD_COLOR);
}
