#pragma once
#pragma once
#include "DataSource.h"  // 继承自之前定义的 DataSource 基类
#include <filesystem>
#include <vector>
#include <random>

class FolderDataSource : public DataSource {
public:
	/**
	* @param folderPath 图片文件夹路径
	* @param fps 模拟帧率（控制读取速度）
	* @param randomOrder 是否随机顺序读取
	*/
	explicit FolderDataSource(const std::string& folderPath,
		double fps = 30.0,
		bool randomOrder = false);

	// 设置播放速度（动态调整）
	void SetPlaybackSpeed(double fps);

protected:
	// 实现基类要求的原始数据获取方法
	cv::Mat _FetchRawImage() override;

private:
	void _LoadImagePaths();
	std::experimental::filesystem::path _GetNextImagePath();

	std::string _folderPath;
	std::vector<std::experimental::filesystem::path> _imagePaths;
	size_t _currentIndex = 0;
	double _fps;
	bool _randomOrder;

	std::mt19937 _rng;
	std::chrono::time_point<std::chrono::system_clock> _lastFrameTime;
};