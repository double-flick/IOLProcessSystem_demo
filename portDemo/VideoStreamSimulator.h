#pragma once

#include "VideoFlowDataSource.h"
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

class VideoStreamSimulator {
public:
	// ���캯����������Ƶ�ļ�·����Ŀ��֡��
	explicit VideoStreamSimulator(const std::string& videoPath, double targetFps = 30.0);

	// ����ģ��������ʼ��ȡ��Ƶ�ļ����ṩͼ��
	void StartSimulation(VideoFlowDataSource& videoFlowDataSource);

	// ֹͣģ����
	void StopSimulation();

private:
	// ����Ƶ�ļ��ж�ȡ֡�������ṩ�� VideoFlowDataSource
	void ReadAndFeedFrames(VideoFlowDataSource& videoFlowDataSource);

	std::string _videoPath;        // ��Ƶ�ļ�·��
	double _targetFps;            // Ŀ��֡�ʣ����� 30Hz��
	std::atomic<bool> _running;    // �����߳����еı�־
	std::thread _simulationThread; // ���ڶ�ȡ���ṩͼ����߳�
};
