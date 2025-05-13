#pragma once
#pragma once
#include "ImagePacket.h"
#include "NetworkManager.h"

class ProtocolSender {
public:
	explicit ProtocolSender(NetworkManager& networkManager);

	// ����ͼ�����ݰ�
	void Send(const ImagePacket& packet);

private:
	NetworkManager& _networkManager;
};