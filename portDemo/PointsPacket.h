#pragma once
#pragma once
#include <vector>
#include <string>
#include <utility> // for std::pair
#include <cstdint>

// ��������ݰ��ṹ������˷��͵����ݰ��������Ϊ�����ʽ
struct PointsPacket {
	std::string imageId;       // ͼ��ID
	std::vector<std::pair<int32_t, int32_t>> points; // ���������
};