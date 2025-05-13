#pragma once
#pragma once
#include "DataSource.h"
#include <string>
#include <vector>

class FileDataSource : public DataSource {
public:
	explicit FileDataSource(const std::string& folderPath);

protected:
	cv::Mat _FetchRawImage() override;

private:
	std::string _folderPath; // 图像文件夹路径
	std::vector<std::string> _imageFiles; // 图像文件列表

	void LoadImages(); // 加载文件夹中的所有图像文件
};
