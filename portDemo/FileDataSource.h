// FileDataSource.h
#pragma once

#include "DataSource.h"
#include <vector>
#include <string>
#include <atomic>
#include <thread>

class FileDataSource : public DataSource {
public:
	explicit FileDataSource(const std::string& folderPath);
	~FileDataSource();

	// ��ֹ����
	FileDataSource(const FileDataSource&) = delete;
	FileDataSource& operator=(const FileDataSource&) = delete;

private:
	void LoadImagePaths();                      // �����ļ����е�ͼ��·��
	void PreloadCacheThreadFunc();              // Ԥ���ػ����̺߳���
	cv::Mat _FetchRawImage() override;          // ʵ�ֻ���Ĵ��麯��

	std::string _GetNextFilePath();             // ��ȡ��һ��Ҫ���ص��ļ�·��

	std::string _folderPath;                   // ͼ���ļ���·��
	std::vector<std::string> _pendingImageFiles; // �����ص�ͼ���ļ��б�
	std::atomic<bool> _stopPreload{ false };    // ֹͣԤ���ر�־
	std::thread _preloadThread;                 // Ԥ�����߳�
	std::mutex _fileListMutex;                  // ����_pendingImageFiles�Ļ�����
};