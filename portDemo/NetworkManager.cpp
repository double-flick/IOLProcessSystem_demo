#include "NetworkManager.h"
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

NetworkManager::NetworkManager() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		throw std::runtime_error("WSAStartup failed");
	}
}

NetworkManager::~NetworkManager() {
	Disconnect();
	WSACleanup();
}

bool NetworkManager::Connect(const std::string& ip, int port) {
	if (_connected) return true;

	// ����socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET) {
		if (_errorCallback) _errorCallback("Socket creation failed");
		return false;
	}
	// ����Nagle�㷨
	int flag = 1;
	setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));

	// ���÷�������ַ
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

	// ���ӷ�����
	if (connect(_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(_socket);
		_socket = INVALID_SOCKET;
		if (_errorCallback) _errorCallback("Connection failed");
		return false;
	}

	_connected = true;
	_running = true;

	// ���������߳�
	_networkThread = std::thread(&NetworkManager::NetworkThreadFunc, this);
	_receiveThread = std::thread(&NetworkManager::ReceiveThreadFunc, this);

	return true;
}

void NetworkManager::Disconnect() {
	_running = false;
	_connected = false;

	// ���ѿ��ܵȴ����߳�
	_queueCV.notify_all();

	// �ر�socket
	if (_socket != INVALID_SOCKET) {
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}

	// �ȴ��߳̽���
	if (_networkThread.joinable()) _networkThread.join();
	if (_receiveThread.joinable()) _receiveThread.join();

	// ��ն���
	std::lock_guard<std::mutex> lock(_queueMutex);
	while (!_sendQueue.empty()) _sendQueue.pop();
}

bool NetworkManager::IsConnected() const {
	return _connected;
}

void NetworkManager::SendData(const std::vector<char>& data) {
	if (!_connected) return;

	std::lock_guard<std::mutex> lock(_queueMutex);
	_sendQueue.push(data);
	_queueCV.notify_one();
}

void NetworkManager::NetworkThreadFunc() {
	while (_running) {
		std::unique_lock<std::mutex> lock(_queueMutex);
		_queueCV.wait(lock, [this] { return !_sendQueue.empty() || !_running; });

		if (!_running) break;

		auto data = std::move(_sendQueue.front());
		_sendQueue.pop();
		lock.unlock();

		// ��������
		int totalSent = 0;
		while (totalSent < data.size() && _connected) {
			int sent = send(_socket, data.data() + totalSent, data.size() - totalSent, 0);
			if (sent == SOCKET_ERROR) {
				_connected = false;
				if (_errorCallback) _errorCallback("Send failed");
				break;
			}
			totalSent += sent;
		}
	}
}

void NetworkManager::ReceiveThreadFunc() {
	std::vector<char> buffer(4096);

	while (_running && _connected) {
		int received = recv(_socket, buffer.data(), buffer.size(), 0);
		if (received <= 0) {
			_connected = false;
			if (_errorCallback) _errorCallback("Connection closed");
			break;
		}

		if (_dataCallback) {
			_dataCallback(std::vector<char>(buffer.begin(), buffer.begin() + received));
		}
	}
}

void NetworkManager::SetDataCallback(DataCallback callback) {
	_dataCallback = std::move(callback);
}

void NetworkManager::SetErrorCallback(ErrorCallback callback) {
	_errorCallback = std::move(callback);
}