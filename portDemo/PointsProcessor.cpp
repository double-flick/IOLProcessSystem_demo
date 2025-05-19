// PointsProcessor.cpp
#include "PointsProcessor.h"
#include <iostream>
#include <filesystem> // ���ڴ���Ŀ¼

/*�ص�������һ�����յ����ݾʹ�����
�����յ������ݽ���Ϊͼ��id�͹ؼ�������󣬻��Ʋ����浽���أ����ڹ۲�����Ч��*/

namespace fs = std::experimental::filesystem;

PointsProcessor::PointsProcessor(DataSource& dataSource, const std::string& outputFolder)
	: _dataSource(dataSource), _outputFolder(outputFolder) {

	// ȷ�����Ŀ¼����
	if (!fs::exists(_outputFolder)) {
		fs::create_directories(_outputFolder);
	}
}

void PointsProcessor::ProcessPointsPacket(const PointsPacket& packet) {
	// 1. ������Դ�����л�ȡԭͼ
	cv::Mat originalImage;
	if (!_dataSource.GetOriginalImage(packet.imageId, originalImage)) {
		std::cerr << "���棺�Ҳ���ͼ��ID��Ӧ��ԭͼ: " << packet.imageId << std::endl;
		return;
	}

	// //2. ����ͼ�񸱱����ڻ��ƣ������޸Ļ����е�ԭͼ��
	//cv::Mat imageWithPoints = originalImage.clone();

	//// 3. ��������㵽ͼ����
	//DrawPointsOnImage(packet, imageWithPoints);

	//// 4. ���ɱ���·��������ͼ��
	//std::string outputPath = GenerateOutputPath(packet.imageId);
	////std::cout << "���·��: " << outputPath << std::endl;
	//if (!cv::imwrite(outputPath, imageWithPoints)) {
	//	std::cerr << "�����޷�����ͼ�� " << outputPath << std::endl;
	//}
	//else {
	//	//std::cout << "�ѱ��洦����ͼ��: " << outputPath << std::endl;
	//}

	// 5. �ͷŻ���
	_dataSource.ReleaseImage(packet.imageId);

	// 6. ��¼����������ʱ
	LogProcessingTime(packet.imageId);

	// 7. ��ʾͼ�񣨿�ѡ��
	//cv::imshow("������Ԥ��", imageWithPoints);
	//cv::waitKey(1);
}

void PointsProcessor::DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image) {
	const cv::Scalar redColor(0, 0, 255); // ��ɫ
	const cv::Scalar blueColor(255, 0, 0); // ��ɫ
	const cv::Scalar greenColor(0, 255, 0); // ��ɫ
	const int radius = 5;
	const int thickness = 2; // ����Բ

	int pointCount = packet.points.size();
	int redPoints = 2;
	int bluePoints = 7;
	int greenPoints = 7;

	for (size_t i = 0; i < pointCount; ++i) {
		const auto& point = packet.points[i];
		cv::Scalar color;

		if (i < redPoints) {
			color = redColor; // ǰ�������ú�ɫ
		}
		else if (i < redPoints + bluePoints) {
			color = blueColor; // ������7��������ɫ
		}
		else {
			color = greenColor; // ���7��������ɫ
		}

		cv::circle(image,
			cv::Point(point.first, point.second),
			radius, color, thickness);
	}
}

std::string PointsProcessor::GenerateOutputPath(const std::string& imageId) const {
	// �������磺output_folder/image_id_points.jpg ��·��
	return _outputFolder + "/" + imageId + "_points.jpg";
}

// �������������㲢��������ʱ
void PointsProcessor::LogProcessingTime(const std::string& imageId) {
	// ��ȡ��ǰʱ���
	auto now = std::chrono::high_resolution_clock::now();
	auto nowTime = now.time_since_epoch().count();

	// ��ȡͼƬID�е�ʱ���
	// ʹ��std::stoullȷ�����Դ�����������
	uint64_t imageIdTime = std::stoull(imageId);

	// �����ʱ����λ�����룩
	auto processingTime = nowTime - imageIdTime;

	// ת��Ϊ��
	double processingTimeInSeconds = static_cast<double>(processingTime) / 1e9;

	// �����ʱ
	std::cout << "����ͼ�� " << imageId << " ���ܺ�ʱ: " << processingTimeInSeconds << " ��" << std::endl;
}