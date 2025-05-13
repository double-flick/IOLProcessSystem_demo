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
	// 回调函数类型定义
	using DataCallback = std::function<void(const std::vector<char>&)>;
	using ErrorCallback = std::function<void(const std::string&)>;

	NetworkManager();
	~NetworkManager();

	// 连接/断开
	bool Connect(const std::string& ip, int port);
	void Disconnect();
	bool IsConnected() const;

	// 数据发送
	void SendData(const std::vector<char>& data);

	// 设置回调
	void SetDataCallback(DataCallback callback);
	void SetErrorCallback(ErrorCallback callback);

private:
	// 网络线程函数
	void NetworkThreadFunc();
	void ReceiveThreadFunc();

	// 连接状态
	std::atomic<bool> _running{ false };
	std::atomic<bool> _connected{ false };
	SOCKET _socket{ INVALID_SOCKET };

	// 线程控制
	std::thread _networkThread;
	std::thread _receiveThread;

	// 数据队列
	std::queue<std::vector<char>> _sendQueue;
	std::mutex _queueMutex;
	std::condition_variable _queueCV;

	// 回调函数
	DataCallback _dataCallback;
	ErrorCallback _errorCallback;
};
