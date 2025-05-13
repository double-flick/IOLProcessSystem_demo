#pragma once
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// 包含图像数据和元信息
struct ImagePacket {
	std::vector<uchar> encodedData;  // 编码后的图像数据
	std::string imageId;             // 唯一标识符（如时间戳/UUID）
	int width = 0;                   // 图像宽（可选）
	int height = 0;                  // 图像高（可选）
};

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

private:
	// 生成唯一ID（示例：时间戳+随机数）
	static std::string GenerateImageId(const std::string& prefix);
};