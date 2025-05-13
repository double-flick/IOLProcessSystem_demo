#include "ProtocolSender.h"
#include <cstring>
#include <sstream>

ProtocolSender::ProtocolSender(NetworkManager& networkManager)
	: _networkManager(networkManager) {}

void ProtocolSender::Send(const ImagePacket& packet) {
	// ���л�ͼ�����ݰ�
	std::vector<char> buffer;

	// ������ݳ���
	buffer.resize(sizeof(packet.dataSize));
	std::memcpy(buffer.data(), &packet.dataSize, sizeof(packet.dataSize));

	// ���ͼ��ID
	std::string imageId = packet.imageId;
	buffer.insert(buffer.end(), imageId.begin(), imageId.end());
	buffer.push_back('\0'); // ����ַ���������


	// ���ͼ������
	buffer.insert(buffer.end(), packet.data.begin(), packet.data.end());

	// �������ݰ�
	_networkManager.SendData(buffer);
}