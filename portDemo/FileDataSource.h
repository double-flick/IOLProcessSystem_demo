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
	std::string _folderPath; // ͼ���ļ���·��
	std::vector<std::string> _imageFiles; // ͼ���ļ��б�

	void LoadImages(); // �����ļ����е�����ͼ���ļ�
};
