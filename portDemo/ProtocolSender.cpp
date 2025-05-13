#include "ProtocolSender.h"
#include <cstring>
#include <sstream>

ProtocolSender::ProtocolSender(NetworkManager& networkManager)
	: _networkManager(networkManager) {}

void ProtocolSender::Send(const ImagePacket& packet) {
	// ���л�ͼ�����ݰ�
	std::vector<char> buffer;

	// ������ݳ��ȣ�ת��Ϊ�����ֽ���
	uint32_t dataSizeNetwork = htonl(packet.dataSize);
	buffer.resize(sizeof(dataSizeNetwork));
	std::memcpy(buffer.data(), &dataSizeNetwork, sizeof(dataSizeNetwork));

	// ���ͼ��ID
	std::string imageId = packet.imageId;
	buffer.insert(buffer.end(), imageId.begin(), imageId.end());
	buffer.push_back('\0'); // ����ַ���������


	// ���ͼ������
	buffer.insert(buffer.end(), packet.data.begin(), packet.data.end());

	// �������ݰ�
	_networkManager.SendData(buffer);
}