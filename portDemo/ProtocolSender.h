#pragma once
#pragma once
#include "ImagePacket.h"
#include "NetworkManager.h"

class ProtocolSender {
public:
	explicit ProtocolSender(NetworkManager& networkManager);

	// 发送图像数据包
	void Send(const ImagePacket& packet);

private:
	NetworkManager& _networkManager;
};