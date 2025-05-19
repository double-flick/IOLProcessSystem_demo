// PointsProcessor.cpp
#include "PointsProcessor.h"
#include <iostream>
#include <filesystem> // 用于创建目录

/*回调函数，一旦接收到数据就触发，
将接收到的数据解码为图像id和关键点坐标后，绘制并保存到本地，用于观察推理效果*/

namespace fs = std::experimental::filesystem;

PointsProcessor::PointsProcessor(DataSource& dataSource, const std::string& outputFolder)
	: _dataSource(dataSource), _outputFolder(outputFolder) {

	// 确保输出目录存在
	if (!fs::exists(_outputFolder)) {
		fs::create_directories(_outputFolder);
	}
}

void PointsProcessor::ProcessPointsPacket(const PointsPacket& packet) {
	// 1. 从数据源缓存中获取原图
	cv::Mat originalImage;
	if (!_dataSource.GetOriginalImage(packet.imageId, originalImage)) {
		std::cerr << "警告：找不到图像ID对应的原图: " << packet.imageId << std::endl;
		return;
	}

	// //2. 创建图像副本用于绘制（避免修改缓存中的原图）
	//cv::Mat imageWithPoints = originalImage.clone();

	//// 3. 绘制坐标点到图像上
	//DrawPointsOnImage(packet, imageWithPoints);

	//// 4. 生成保存路径并保存图像
	//std::string outputPath = GenerateOutputPath(packet.imageId);
	////std::cout << "输出路径: " << outputPath << std::endl;
	//if (!cv::imwrite(outputPath, imageWithPoints)) {
	//	std::cerr << "错误：无法保存图像到 " << outputPath << std::endl;
	//}
	//else {
	//	//std::cout << "已保存处理后的图像到: " << outputPath << std::endl;
	//}

	// 5. 释放缓存
	_dataSource.ReleaseImage(packet.imageId);

	// 6. 记录并输出处理耗时
	LogProcessingTime(packet.imageId);

	// 7. 显示图像（可选）
	//cv::imshow("处理结果预览", imageWithPoints);
	//cv::waitKey(1);
}

void PointsProcessor::DrawPointsOnImage(const PointsPacket& packet, cv::Mat& image) {
	const cv::Scalar redColor(0, 0, 255); // 红色
	const cv::Scalar blueColor(255, 0, 0); // 蓝色
	const cv::Scalar greenColor(0, 255, 0); // 绿色
	const int radius = 5;
	const int thickness = 2; // 空心圆

	int pointCount = packet.points.size();
	int redPoints = 2;
	int bluePoints = 7;
	int greenPoints = 7;

	for (size_t i = 0; i < pointCount; ++i) {
		const auto& point = packet.points[i];
		cv::Scalar color;

		if (i < redPoints) {
			color = redColor; // 前两个点用红色
		}
		else if (i < redPoints + bluePoints) {
			color = blueColor; // 接下来7个点用蓝色
		}
		else {
			color = greenColor; // 最后7个点用绿色
		}

		cv::circle(image,
			cv::Point(point.first, point.second),
			radius, color, thickness);
	}
}

std::string PointsProcessor::GenerateOutputPath(const std::string& imageId) const {
	// 生成形如：output_folder/image_id_points.jpg 的路径
	return _outputFolder + "/" + imageId + "_points.jpg";
}

// 新增方法：计算并输出处理耗时
void PointsProcessor::LogProcessingTime(const std::string& imageId) {
	// 获取当前时间戳
	auto now = std::chrono::high_resolution_clock::now();
	auto nowTime = now.time_since_epoch().count();

	// 获取图片ID中的时间戳
	// 使用std::stoull确保可以处理更大的数字
	uint64_t imageIdTime = std::stoull(imageId);

	// 计算耗时（单位：纳秒）
	auto processingTime = nowTime - imageIdTime;

	// 转换为秒
	double processingTimeInSeconds = static_cast<double>(processingTime) / 1e9;

	// 输出耗时
	std::cout << "处理图像 " << imageId << " 的总耗时: " << processingTimeInSeconds << " 秒" << std::endl;
}