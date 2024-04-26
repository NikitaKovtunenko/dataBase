#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <queue>
#include <string>

#pragma comment(lib, "Ws2_32.lib")



class Server {

    std::queue<std::string> queStr;
    SOCKET listenSocket;


public:
    Server() = default;
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    int initServer()
    {
        // Инициализация Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return 1;
        }

        // Создание сокета
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        // Настройка адреса сервера
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на всех интерфейсах
        serverAddress.sin_port = htons(8888); // Порт сервера

        // Привязка сокета к адресу
        if (bind(listenSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Начало прослушивания соединений
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Server listening on port 8888" << std::endl;
    }

    int startWork() {
        // Принятие соединений

        std::string confirm = "Hello client";
        std::string msg;
        msg.resize(1024);
        while (true) {
            SOCKET clientSocket = accept(listenSocket, NULL, NULL);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }

            std::cout << "Client connected" << std::endl;

            // Обработка данных от клиента (здесь пример простого эхо-сервера)
            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0) {
                msg = buffer;
                queStr.push(std::move(msg));
                std::cout << msg << std::endl;
                send(clientSocket, confirm.data(), confirm.size(), 0);
            }

            // Закрытие соединения с клиентом
            closesocket(clientSocket);
        }
    }

    ~Server()
    {
        // Очистка
        closesocket(listenSocket);
        WSACleanup();

    }


};

int main() {

    Server server;
    server.initServer();
    server.startWork();



    return 0;
}