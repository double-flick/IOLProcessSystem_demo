#pragma once
#pragma once
#include <vector>
#include <string>
#include <utility> // for std::pair

// ��������ݰ��ṹ������˷��͵����ݰ��������Ϊ�����ʽ
struct PointsPacket {
	std::string imageId;       // ͼ��ID
	std::vector<std::pair<float, float>> points; // ���������
};