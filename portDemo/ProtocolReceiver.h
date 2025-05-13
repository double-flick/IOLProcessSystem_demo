#pragma once
#include "NetworkManager.h"
#include "PointsPacket.h"
#include <functional>
#include <vector>
#include <string>

class ProtocolReceiver {
public:
	using MessageCallback = std::function<void(const PointsPacket&)>;

	explicit ProtocolReceiver(NetworkManager& networkManager);

	// ������Ϣ�ص�����
	void SetMessageCallback(MessageCallback callback);

private:
	NetworkManager& _networkManager;
	MessageCallback _messageCallback;

	std::vector<char> _buffer; // �����ݴ���յ�����
	size_t _bytesToReceive;    // ��ǰ��Ҫ���յ��ֽ���
	bool _isHeaderReceived;    // �Ƿ��Ѿ����յ�����Ϣͷ

	void OnDataReceived(const std::vector<char>& data);
	void ProcessMessage(const PointsPacket& message);
};