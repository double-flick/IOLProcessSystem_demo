#include "ImageProcessor.h"
#include <chrono>
#include <random>
#include <sstream>

ImagePacket ImageProcessor::EncodeImage(const cv::Mat& image, const std::string& id) {
	ImagePacket packet;

	// ����ΪJPEG���ɵ�������������
	cv::imencode(".jpg", image, packet.data, { cv::IMWRITE_JPEG_QUALITY, 90 });

	// �������ݳ���
	packet.dataSize = packet.data.size(); // �������ݳ���Ϊ��������ݵĴ�С

	// ��¼ͼ����Ϣ
	packet.imageId = id;

	return packet;
}

cv::Mat ImageProcessor::DecodeImage(const std::vector<uchar>& data) {
	return cv::imdecode(data, cv::IMREAD_COLOR);
}
