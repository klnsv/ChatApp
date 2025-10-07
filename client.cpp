#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;


void receive_messages(SOCKET sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }
        buffer[valread] = '\0';
        std::cout << "\rOther client: " << buffer << std::endl;
        std::cout << "You: ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }


    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server." << std::endl;

    std::thread receive_thread(receive_messages, sock);
    receive_thread.detach();

    std::string message;
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, message);
        if (message.empty()) continue;
        send(sock, message.c_str(), message.length(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}