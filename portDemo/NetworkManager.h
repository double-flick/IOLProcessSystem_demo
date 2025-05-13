#pragma once
#include <winsock2.h>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>

class NetworkManager {
public:
	// �ص��������Ͷ���
	using DataCallback = std::function<void(const std::vector<char>&)>;
	using ErrorCallback = std::function<void(const std::string&)>;

	NetworkManager();
	~NetworkManager();

	// ����/�Ͽ�
	bool Connect(const std::string& ip, int port);
	void Disconnect();
	bool IsConnected() const;

	// ���ݷ���
	void SendData(const std::vector<char>& data);

	// ���ûص�
	void SetDataCallback(DataCallback callback);
	void SetErrorCallback(ErrorCallback callback);

private:
	// �����̺߳���
	void NetworkThreadFunc();
	void ReceiveThreadFunc();

	// ����״̬
	std::atomic<bool> _running{ false };
	std::atomic<bool> _connected{ false };
	SOCKET _socket{ INVALID_SOCKET };

	// �߳̿���
	std::thread _networkThread;
	std::thread _receiveThread;

	// ���ݶ���
	std::queue<std::vector<char>> _sendQueue;
	std::mutex _queueMutex;
	std::condition_variable _queueCV;

	// �ص�����
	DataCallback _dataCallback;
	ErrorCallback _errorCallback;
};
