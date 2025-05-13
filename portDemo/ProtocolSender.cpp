#include "ProtocolSender.h"
#include <cstring>
#include <sstream>

ProtocolSender::ProtocolSender(NetworkManager& networkManager)
	: _networkManager(networkManager) {}

void ProtocolSender::Send(const ImagePacket& packet) {
	// 序列化图像数据包
	std::vector<char> buffer;

	// 添加数据长度（转换为网络字节序）
	uint32_t dataSizeNetwork = htonl(packet.dataSize);
	buffer.resize(sizeof(dataSizeNetwork));
	std::memcpy(buffer.data(), &dataSizeNetwork, sizeof(dataSizeNetwork));

	// 添加图像ID
	std::string imageId = packet.imageId;
	buffer.insert(buffer.end(), imageId.begin(), imageId.end());
	buffer.push_back('\0'); // 添加字符串结束符


	// 添加图像数据
	buffer.insert(buffer.end(), packet.data.begin(), packet.data.end());

	// 发送数据包
	_networkManager.SendData(buffer);
}