#pragma once
#pragma once
#include <vector>
#include <string>
#include <utility> // for std::pair
#include <cstdint>

// 坐标点数据包结构，服务端发送的数据包将会解析为这个格式
struct PointsPacket {
	std::string imageId;       // 图像ID
	std::vector<std::pair<int32_t, int32_t>> points; // 坐标点数据
};