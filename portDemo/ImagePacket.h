#pragma once
#pragma once
#include <vector>
#include <string>

// ͼ�����ݰ��ṹ
struct ImagePacket {
	int32_t dataSize;      // ���ݳ���
	std::string imageId;   // ͼ��ID
	std::vector<unsigned char> data; // ͼ������
};