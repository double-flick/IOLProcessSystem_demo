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

	// 禁止拷贝
	FileDataSource(const FileDataSource&) = delete;
	FileDataSource& operator=(const FileDataSource&) = delete;

private:
	void LoadImagePaths();                      // 加载文件夹中的图像路径
	void PreloadCacheThreadFunc();              // 预加载缓存线程函数
	cv::Mat _FetchRawImage() override;          // 实现基类的纯虚函数

	std::string _GetNextFilePath();             // 获取下一个要加载的文件路径

	std::string _folderPath;                   // 图像文件夹路径
	std::vector<std::string> _pendingImageFiles; // 待加载的图像文件列表
	std::atomic<bool> _stopPreload{ false };    // 停止预加载标志
	std::thread _preloadThread;                 // 预加载线程
	std::mutex _fileListMutex;                  // 保护_pendingImageFiles的互斥锁
};