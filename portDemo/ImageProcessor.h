#pragma once
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "ImagePacket.h"


class ImageProcessor {
public:
	/**
	* ����ͼ������ΨһID
	* @param image ����ͼ��
	* @param idPrefix IDǰ׺��������ͷID��
	* @return �����������ݺ�ID��ImagePacket
	*/
	static ImagePacket EncodeImage(const cv::Mat& image, const std::string& idPrefix = "");

	/**
	* ����ͼ��
	* @param data ��������
	* @return ������ͼ��
	*/
	static cv::Mat DecodeImage(const std::vector<uchar>& data);

};