#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>  // 新增
#include <opencv2/opencv.hpp>

#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed." << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9999);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // 修改处

	if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connect failed." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	cv::Mat image = cv::imread("E:/WORK/mvs/projects/senderDemo/portDemo/imagetest/ddj.jpg");
	if (image.empty()) {
		std::cerr << "Failed to read image." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	std::vector<uchar> buffer;
	cv::imencode(".jpg", image, buffer);

	int dataSize = buffer.size();
	send(clientSocket, (const char*)&dataSize, sizeof(dataSize), 0);
	send(clientSocket, (const char*)buffer.data(), dataSize, 0);

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}