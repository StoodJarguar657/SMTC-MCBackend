#include <MCBackend.hpp>

int main() {
    
    MCBackend backend;
    
    MCServerDesc testServer;
    testServer.serverFolder = "C:\\Users\\emanu\\Desktop\\ServerData\\testServer\\";
    if (!backend.addServer(testServer))
        return 1;

    //MCServerDesc testServer1;
    //testServer1.serverFolder = "C:\\Users\\emanu\\Desktop\\ServerData\\testServer1\\";
    //if (!backend.addServer(testServer1))
    //    return 1;
    //
    //MCServerDesc testServer2;
    //testServer2.serverFolder = "C:\\Users\\emanu\\Desktop\\ServerData\\testServer2\\";
    //if (!backend.addServer(testServer2))
    //    return 1;

    if (!backend.initialize(25576))
        return 1;

    while (true) {
        backend.update();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}

//#include <iostream>
//#include <thread>
//#include <vector>
//#include <mutex>
//#include <cstring> // For memset
//
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")
//
//std::vector<std::string> connection_logs;
//std::mutex log_mutex;
//
//void log_connection(const std::string& ip) {
//    std::lock_guard<std::mutex> lock(log_mutex);
//    connection_logs.push_back(ip);
//    std::cout << "Minecraft client detected: " << ip << std::endl;
//}
//
//void tcp_listener() {
//    
//    WSADATA wsaData;
//    if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
//        return false;
//
//
//    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//    if (server_fd == -1) {
//        std::cerr << "Socket creation failed!" << std::endl;
//        return;
//    }
//
//    sockaddr_in server_addr{};
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
//    server_addr.sin_port = htons(25565);       // Minecraft port
//
//    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//        std::cerr << "Bind failed!" << std::endl;
//        return;
//    }
//
//    if (listen(server_fd, 5) < 0) {
//        std::cerr << "Listen failed!" << std::endl;
//        return;
//    }
//
//    std::cout << "Listening on port 25565..." << std::endl;
//
//    while (true) {
//        sockaddr_in client_addr{};
//        socklen_t client_len = sizeof(client_addr);
//        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
//
//        if (client_socket < 0) {
//            std::cerr << "Accept failed!" << std::endl;
//            continue;
//        }
//
//        std::string client_ip = inet_ntoa(client_addr.sin_addr);
//        log_connection(client_ip);
//
//#ifdef _WIN32
//        closesocket(client_socket);
//#else
//        close(client_socket);
//#endif
//    }
//
//#ifdef _WIN32
//    WSACleanup();
//#endif
//}
//
//int main() {
//    std::thread listener_thread(tcp_listener);
//    listener_thread.join(); // Keep the thread running
//    return 0;
//}