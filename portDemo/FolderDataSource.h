#pragma once
#pragma once
#include "DataSource.h"  // �̳���֮ǰ����� DataSource ����
#include <filesystem>
#include <vector>
#include <random>

class FolderDataSource : public DataSource {
public:
	/**
	* @param folderPath ͼƬ�ļ���·��
	* @param fps ģ��֡�ʣ����ƶ�ȡ�ٶȣ�
	* @param randomOrder �Ƿ����˳���ȡ
	*/
	explicit FolderDataSource(const std::string& folderPath,
		double fps = 30.0,
		bool randomOrder = false);

	// ���ò����ٶȣ���̬������
	void SetPlaybackSpeed(double fps);

protected:
	// ʵ�ֻ���Ҫ���ԭʼ���ݻ�ȡ����
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