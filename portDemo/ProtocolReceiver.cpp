#include "ProtocolReceiver.h"
#include <cstring>
#include <algorithm> // 包含 std::find
#include <iostream>
#include <cstdint>
#include <cstring>
/*接收服务端返回的推理结果，并包装为PointPacket对象，并触发接收到数据的回调函数*/

//python平台默认大端int类型，cpp平台默认小端，需要做转换
int32_t ntoh_int32(const void* data) {
	uint32_t value;
	std::memcpy(&value, data, sizeof(value));
	return static_cast<int32_t>(ntohl(value));
}

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
			dataSize = ntohl(dataSize); // 网络字节序转本地字节序
			_bytesToReceive = sizeof(int32_t) + dataSize; // 总长度 = 头部(4) + 数据长度
			_isHeaderReceived = true;

			std::cout << "Header received, data size: " << dataSize
				<< ", Total bytes to receive: " << _bytesToReceive << std::endl;

			continue; // 继续循环处理当前 buffer
		}

		if (_buffer.size() >= _bytesToReceive) {
			// 解析完整的坐标点数据包
			PointsPacket packet;

			// 获取 buffer 起始地址
			const char* pBuffer = _buffer.data();
			const char* pPayload = pBuffer + sizeof(int32_t); // 跳过数据长度字段

															  // 打印整个 buffer（用于比对）
			//std::cout << "Received buffer (hex): ";
			//for (size_t i = 0; i < _bytesToReceive; ++i) {
			//	printf("%02X ", static_cast<unsigned char>(_buffer[i]));
			//}
			//printf("\n");

			// 解析图像ID
			const char* pIdStart = pPayload;
			const char* pNull = reinterpret_cast<const char*>(std::memchr(pIdStart, '\0', _bytesToReceive - sizeof(int32_t)));
			if (!pNull) {
				std::cerr << "Error: No null terminator found in buffer" << std::endl;
				break;
			}

			packet.imageId = std::string(pIdStart, pNull);
			//std::cout << "Parsed imageId: " << packet.imageId << std::endl;

			// 解析坐标点
			const char* pPointsStart = pNull + 1;

			//std::cout << "pPointsStart address: " << static_cast<const void*>(pPointsStart) << std::endl;
			//std::cout << "Bytes at pPointsStart (hex): ";
			//for (int i = 0; i < 16; ++i) {
			//	printf("%02X ", static_cast<unsigned char>(pPointsStart[i]));
			//}
			//printf("\n");

			size_t pointsBufferSize = _bytesToReceive - (pPointsStart - _buffer.data());
			size_t pointsCount = pointsBufferSize / (2 * sizeof(int32_t)); // 每个点两个 int32_t

			packet.points.resize(pointsCount);

			for (size_t i = 0; i < pointsCount; ++i) {
				const char* pointData = pPointsStart + i * 2 * sizeof(int32_t);

				int32_t x = ntoh_int32(pointData);
				int32_t y = ntoh_int32(pointData + sizeof(int32_t));

				packet.points[i] = { x, y };

				//std::cout << "Point[" << i << "] at " << static_cast<const void*>(pointData)
				//	<< " -> x: " << x << ", y: " << y << std::endl;
			}

			// 调用消息回调函数
			ProcessMessage(packet);

			// 移除已处理的数据
			_buffer.erase(_buffer.begin(), _buffer.begin() + _bytesToReceive);

			// 重置状态
			_bytesToReceive = sizeof(int32_t);
			_isHeaderReceived = false;
		}
	}
}


//void ProtocolReceiver::OnDataReceived(const std::vector<char>& data) {
//	_buffer.insert(_buffer.end(), data.begin(), data.end());
//
//	while (_buffer.size() >= _bytesToReceive) {
//		if (!_isHeaderReceived) {
//			int32_t dataSize;
//			std::memcpy(&dataSize, _buffer.data(), sizeof(dataSize));
//			dataSize = ntohl(dataSize);
//			_bytesToReceive = sizeof(int32_t) + dataSize;
//			_isHeaderReceived = true;
//
//			std::cout << "Header received, data size: " << dataSize
//				<< ", Total bytes to receive: " << _bytesToReceive << std::endl;
//
//			continue;
//		}
//
//		if (_buffer.size() >= _bytesToReceive) {
//			PointsPacket packet;
//
//			const char* pBuffer = _buffer.data();
//			const char* pPayload = pBuffer + sizeof(int32_t);
//
//			const char* pNull = reinterpret_cast<const char*>(std::memchr(pPayload, '\0', _bytesToReceive - sizeof(int32_t)));
//			if (!pNull) {
//				std::cerr << "Error: No null terminator found in buffer" << std::endl;
//				break;
//			}
//
//			packet.imageId = std::string(pPayload, pNull);
//			std::cout << "Parsed imageId: " << packet.imageId << std::endl;
//
//			const char* pPointsStart = pNull + 1;
//			size_t pointsBufferSize = _bytesToReceive - (pPointsStart - pBuffer);
//			size_t pointsCount = pointsBufferSize / (2 * sizeof(int32_t));
//
//			packet.points.resize(pointsCount);
//			for (size_t i = 0; i < pointsCount; ++i) {
//				const char* pointData = pPointsStart + i * 2 * sizeof(int32_t);
//				int32_t x = ntoh_int32(pointData);
//				int32_t y = ntoh_int32(pointData + sizeof(int32_t));
//				packet.points[i] = { x, y };
//			}
//
//			ProcessMessage(packet);
//
//			_buffer.erase(_buffer.begin(), _buffer.begin() + _bytesToReceive);
//			_bytesToReceive = sizeof(int32_t);
//			_isHeaderReceived = false;
//		}
//	}
//}


void ProtocolReceiver::ProcessMessage(const PointsPacket& message) {
	if (_messageCallback) {
		_messageCallback(message);
	}
}