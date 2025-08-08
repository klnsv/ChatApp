#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

void handle_client(SOCKET client_socket, SOCKET other_client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }
        buffer[valread] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        send(other_client_socket, buffer, valread, 0);
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET;
    std::vector<SOCKET> client_sockets(2, INVALID_SOCKET);
    struct sockaddr_in server, client;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }


    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    for (int i = 0; i < 2; ++i) {
        int addrlen = sizeof(client);
        client_sockets[i] = accept(server_fd, (struct sockaddr*)&client, &addrlen);
        if (client_sockets[i] == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }
        std::cout << "Client " << (i + 1) << " connected." << std::endl;
    }


    std::thread t1(handle_client, client_sockets[0], client_sockets[1]);
    std::thread t2(handle_client, client_sockets[1], client_sockets[0]);


    t1.join();
    t2.join();


    for (SOCKET sock : client_sockets) {
        closesocket(sock);
    }
    closesocket(server_fd);
    WSACleanup();
    return 0;
}