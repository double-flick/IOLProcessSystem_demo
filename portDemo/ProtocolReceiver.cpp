#include "ProtocolReceiver.h"
#include <cstring>
#include <algorithm> // ���� std::find

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
			_bytesToReceive = sizeof(dataSize) + dataSize + 1; // �����ַ���������
			_isHeaderReceived = true;
		}

		if (_buffer.size() >= _bytesToReceive) {
			// ������������������ݰ�
			PointsPacket packet;

			// �������ݳ���
			int32_t dataSize;
			std::memcpy(&dataSize, _buffer.data(), sizeof(dataSize));

			// ����ͼ��ID
			auto it = std::find(_buffer.begin() + sizeof(dataSize), _buffer.end(), '\0');
			if (it == _buffer.end()) {
				// ���û���ҵ��ַ�����������˵�����ݲ�����
				break;
			}
			size_t idSize = it - (_buffer.begin() + sizeof(dataSize));
			packet.imageId.assign(_buffer.begin() + sizeof(dataSize), it);
			_buffer.erase(_buffer.begin(), it + 1); // �Ƴ�ͼ��ID���ַ���������

													// �������������
			size_t pointsSize = dataSize / (2 * sizeof(float)); // ÿ����������floatֵ
			packet.points.resize(pointsSize);
			for (size_t i = 0; i < pointsSize; ++i) {
				float x, y;
				std::memcpy(&x, _buffer.data() + i * 2 * sizeof(float), sizeof(float));
				std::memcpy(&y, _buffer.data() + i * 2 * sizeof(float) + sizeof(float), sizeof(float));
				packet.points[i] = { x, y };
			}
			_buffer.erase(_buffer.begin(), _buffer.begin() + dataSize); // �Ƴ����������

																		// ������Ϣ�ص�����
			ProcessMessage(packet);

			// ����״̬
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