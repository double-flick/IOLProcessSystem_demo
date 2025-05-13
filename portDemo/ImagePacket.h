#pragma once
#pragma once
#include <vector>
#include <string>

// 图像数据包结构
struct ImagePacket {
	int32_t dataSize;      // 数据长度
	std::string imageId;   // 图像ID
	std::vector<unsigned char> data; // 图像数据
};