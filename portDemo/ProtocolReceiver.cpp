#include "ProtocolReceiver.h"
#include <cstring>
#include <algorithm> // 包含 std::find

ProtocolReceiver::ProtocolReceiver(NetworkManager& networkManager)
	: _networkManager(networkManager), _bytesToReceive(sizeof(int32_t)), _isHeaderReceived(false) {
	_networkManager.SetDataCallback([this](const std::vector<char>& data) {
		OnDataReceived(data);
	});
}

void ProtocolReceiver::SetMessageCallback(MessageCallback callback) {
	_messageCallback = std::move(callback);
}

void ProtocolReceiver::OnDataReceived(const std::vector<char>& data) {
	// 将接收到的数据添加到缓冲区
	_buffer.insert(_buffer.end(), data.begin(), data.end());

	while (_buffer.size() >= _bytesToReceive) {
		if (!_isHeaderReceived) {
			// 解析数据长度
			int32_t dataSize;
			std::memcpy(&dataSize, _buffer.data(), sizeof(dataSize));
			_bytesToReceive = sizeof(dataSize) + dataSize + 1; // 包括字符串结束符
			_isHeaderReceived = true;
		}

		if (_buffer.size() >= _bytesToReceive) {
			// 解析完整的坐标点数据包
			PointsPacket packet;

			// 解析数据长度
			int32_t dataSize;
			std::memcpy(&dataSize, _buffer.data(), sizeof(dataSize));

			// 解析图像ID
			auto it = std::find(_buffer.begin() + sizeof(dataSize), _buffer.end(), '\0');
			if (it == _buffer.end()) {
				// 如果没有找到字符串结束符，说明数据不完整
				break;
			}
			size_t idSize = it - (_buffer.begin() + sizeof(dataSize));
			packet.imageId.assign(_buffer.begin() + sizeof(dataSize), it);
			_buffer.erase(_buffer.begin(), it + 1); // 移除图像ID和字符串结束符

													// 解析坐标点数据
			size_t pointsSize = dataSize / (2 * sizeof(float)); // 每个点有两个float值
			packet.points.resize(pointsSize);
			for (size_t i = 0; i < pointsSize; ++i) {
				float x, y;
				std::memcpy(&x, _buffer.data() + i * 2 * sizeof(float), sizeof(float));
				std::memcpy(&y, _buffer.data() + i * 2 * sizeof(float) + sizeof(float), sizeof(float));
				packet.points[i] = { x, y };
			}
			_buffer.erase(_buffer.begin(), _buffer.begin() + dataSize); // 移除坐标点数据

																		// 调用消息回调函数
			ProcessMessage(packet);

			// 重置状态
			_bytesToReceive = sizeof(int32_t);
			_isHeaderReceived = false;
		}
	}
}

void ProtocolReceiver::ProcessMessage(const PointsPacket& message) {
	if (_messageCallback) {
		_messageCallback(message);
	}
}