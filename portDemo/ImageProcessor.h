#pragma once
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// ����ͼ�����ݺ�Ԫ��Ϣ
struct ImagePacket {
	std::vector<uchar> encodedData;  // ������ͼ������
	std::string imageId;             // Ψһ��ʶ������ʱ���/UUID��
	int width = 0;                   // ͼ�����ѡ��
	int height = 0;                  // ͼ��ߣ���ѡ��
};

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

private:
	// ����ΨһID��ʾ����ʱ���+�������
	static std::string GenerateImageId(const std::string& prefix);
};