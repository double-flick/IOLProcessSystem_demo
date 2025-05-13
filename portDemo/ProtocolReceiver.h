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

	// 设置消息回调函数
	void SetMessageCallback(MessageCallback callback);

private:
	NetworkManager& _networkManager;
	MessageCallback _messageCallback;

	std::vector<char> _buffer; // 用于暂存接收的数据
	size_t _bytesToReceive;    // 当前需要接收的字节数
	bool _isHeaderReceived;    // 是否已经接收到了消息头

	void OnDataReceived(const std::vector<char>& data);
	void ProcessMessage(const PointsPacket& message);
};