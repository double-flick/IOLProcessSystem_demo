#include "ProtocolReceiver.h"
#include <cstring>
#include <algorithm> // ���� std::find
#include <iostream>
#include <cstdint>
#include <cstring>
/*���շ���˷��ص�������������װΪPointPacket���󣬲��������յ����ݵĻص�����*/

//pythonƽ̨Ĭ�ϴ��int���ͣ�cppƽ̨Ĭ��С�ˣ���Ҫ��ת��
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
	// �����յ���������ӵ�������
	_buffer.insert(_buffer.end(), data.begin(), data.end());

	while (_buffer.size() >= _bytesToReceive) {
		if (!_isHeaderReceived) {
			// �������ݳ���
			int32_t dataSize;
			std::memcpy(&dataSize, _buffer.data(), sizeof(dataSize));
			dataSize = ntohl(dataSize); // �����ֽ���ת�����ֽ���
			_bytesToReceive = sizeof(int32_t) + dataSize; // �ܳ��� = ͷ��(4) + ���ݳ���
			_isHeaderReceived = true;

			std::cout << "Header received, data size: " << dataSize
				<< ", Total bytes to receive: " << _bytesToReceive << std::endl;

			continue; // ����ѭ������ǰ buffer
		}

		if (_buffer.size() >= _bytesToReceive) {
			// ������������������ݰ�
			PointsPacket packet;

			// ��ȡ buffer ��ʼ��ַ
			const char* pBuffer = _buffer.data();
			const char* pPayload = pBuffer + sizeof(int32_t); // �������ݳ����ֶ�

															  // ��ӡ���� buffer�����ڱȶԣ�
			//std::cout << "Received buffer (hex): ";
			//for (size_t i = 0; i < _bytesToReceive; ++i) {
			//	printf("%02X ", static_cast<unsigned char>(_buffer[i]));
			//}
			//printf("\n");

			// ����ͼ��ID
			const char* pIdStart = pPayload;
			const char* pNull = reinterpret_cast<const char*>(std::memchr(pIdStart, '\0', _bytesToReceive - sizeof(int32_t)));
			if (!pNull) {
				std::cerr << "Error: No null terminator found in buffer" << std::endl;
				break;
			}

			packet.imageId = std::string(pIdStart, pNull);
			//std::cout << "Parsed imageId: " << packet.imageId << std::endl;

			// ���������
			const char* pPointsStart = pNull + 1;

			//std::cout << "pPointsStart address: " << static_cast<const void*>(pPointsStart) << std::endl;
			//std::cout << "Bytes at pPointsStart (hex): ";
			//for (int i = 0; i < 16; ++i) {
			//	printf("%02X ", static_cast<unsigned char>(pPointsStart[i]));
			//}
			//printf("\n");

			size_t pointsBufferSize = _bytesToReceive - (pPointsStart - _buffer.data());
			size_t pointsCount = pointsBufferSize / (2 * sizeof(int32_t)); // ÿ�������� int32_t

			packet.points.resize(pointsCount);

			for (size_t i = 0; i < pointsCount; ++i) {
				const char* pointData = pPointsStart + i * 2 * sizeof(int32_t);

				int32_t x = ntoh_int32(pointData);
				int32_t y = ntoh_int32(pointData + sizeof(int32_t));

				packet.points[i] = { x, y };

				//std::cout << "Point[" << i << "] at " << static_cast<const void*>(pointData)
				//	<< " -> x: " << x << ", y: " << y << std::endl;
			}

			// ������Ϣ�ص�����
			ProcessMessage(packet);

			// �Ƴ��Ѵ��������
			_buffer.erase(_buffer.begin(), _buffer.begin() + _bytesToReceive);

			// ����״̬
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