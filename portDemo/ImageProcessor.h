#pragma once
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "ImagePacket.h"


class ImageProcessor {
public:
	/**
	* 编码图像并生成唯一ID
	* @param image 输入图像
	* @param idPrefix ID前缀（如摄像头ID）
	* @return 包含编码数据和ID的ImagePacket
	*/
	static ImagePacket EncodeImage(const cv::Mat& image, const std::string& idPrefix = "");

	/**
	* 解码图像
	* @param data 编码数据
	* @return 解码后的图像
	*/
	static cv::Mat DecodeImage(const std::vector<uchar>& data);

};