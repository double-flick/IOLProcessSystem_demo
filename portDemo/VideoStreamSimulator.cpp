#include "VideoStreamSimulator.h"
#include <iostream>
#include <chrono>

// ���캯������ʼ����Ƶ�ļ�·����Ŀ��֡��
VideoStreamSimulator::VideoStreamSimulator(const std::string& videoPath, double targetFps)
	: _videoPath(videoPath), _targetFps(targetFps), _running(false) {}

// ����ģ��������ʼ��ȡ��Ƶ�ļ����ṩͼ��
void VideoStreamSimulator::StartSimulation(VideoFlowDataSource& videoFlowDataSource) {
	_running = true;
	_simulationThread = std::thread(&VideoStreamSimulator::ReadAndFeedFrames, this, std::ref(videoFlowDataSource));
}

// ֹͣģ����
void VideoStreamSimulator::StopSimulation() {
	_running = false;
	if (_simulationThread.joinable()) {
		_simulationThread.join(); // �ȴ��߳̽���
	}
}

// ����Ƶ�ļ��ж�ȡ֡�������ṩ�� VideoFlowDataSource
void VideoStreamSimulator::ReadAndFeedFrames(VideoFlowDataSource& videoFlowDataSource) {
	cv::VideoCapture videoCapture(_videoPath);
	if (!videoCapture.isOpened()) {
		std::cerr << "Error: Could not open video file: " << _videoPath << std::endl;
		return;
	}

	double fps = videoCapture.get(cv::CAP_PROP_FPS);  // ��ȡ��Ƶ��ʵ��֡�ʣ�60Hz��
	std::cout << "Video FPS: " << fps << std::endl;

	cv::Mat frame;
	int frameCount = 0;

	while (_running) {
		// ��ȡһ֡
		if (!videoCapture.read(frame)) {
			std::cerr << "Error: Could not read frame from video file." << std::endl;
			break;
		}

		// �ṩͼ��� VideoFlowDataSource��ÿ�� 30 ֡��30Hz��
		if (frameCount % (int)(fps / _targetFps) == 0) {
			videoFlowDataSource.AddImage(frame);  // ��ͼ����ӵ� VideoFlowDataSource
		}

		frameCount++;

		// ���ƶ�ȡ���ʣ���֤ 30Hz �ṩͼ��
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / _targetFps)));
	}

	videoCapture.release();
}
