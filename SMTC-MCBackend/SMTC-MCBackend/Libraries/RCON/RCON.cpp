#include "pch.h"
#include <RCON.hpp>

#include <thread>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#define closesocket close;
#define SD_SEND SHUT_RDWR
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

void RCON::Packet::set(int reqId, int packetType, const std::string& data) {
    this->requestId = reqId;
    this->type = packetType;
    this->payload = data;
    this->length = 4 + 4 + static_cast<int>(payload.size()) + 2;
}

std::string RCON::Packet::serialize() {
    std::string packet(this->length + 4, '\0');

    int offset = 0;

    std::memcpy(&packet[offset], &this->length, 4);
    offset += 4;

    std::memcpy(&packet[offset], &this->requestId, 4);
    offset += 4;

    std::memcpy(&packet[offset], &this->type, 4);
    offset += 4;

    std::memcpy(&packet[offset], payload.c_str(), static_cast<int>(payload.size()));
    offset += static_cast<int>(payload.size());

    return packet;
}

void RCON::Packet::deserialize(const char* buffer, int bytesRead) {
    if (bytesRead < 12)
        return;

    int offset = 0;

    // Length
    std::memcpy(&length, &buffer[offset], 4);
    offset += 4;

    // Request ID
    std::memcpy(&requestId, &buffer[offset], 4);
    offset += 4;

    // Type
    std::memcpy(&type, &buffer[offset], 4);
    offset += 4;

    // Payload
    payload = std::string(&buffer[offset], bytesRead - offset - 2); // Exclude the null terminators
}

bool RCON::init(const std::string& ip, int port) {

    this->ip = ip;
    this->port = port;

    this->socketHandle = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
    if (this->socketHandle < 0) {
        printf("[RCON -> RCON] Socket creation failed\n");
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <= 0) {
        printf("[RCON -> RCON] Invalid address\n");
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        return false;
    }
    
    if (connect(this->socketHandle, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        printf("[RCON -> RCON] Connection failed\n");
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        return false;
    }

    return true;
}

bool RCON::authenticate(const std::string& password) {

    int requestId = 1;
    Packet authPacket;
    authPacket.set(requestId, PACKET_TYPE_AUTH, password);

    const std::string& serializedPacket = authPacket.serialize();
    if (send(this->socketHandle, serializedPacket.c_str(), static_cast<int>(serializedPacket.size()), 0) < 0) {
        printf("[RCON -> Authenticate] Failed to send auth packet\n");
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        this->socketHandle = false;
        this->init(this->ip, this->port);
        return false;
    }

    char buffer[4096] = { 0 };
    int bytesRead = recv(this->socketHandle, buffer, 4096, 0);
    if (bytesRead <= 0) {
        printf("[RCON -> Authenticate] Failed to receive response\n");
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        return false;
    }

    // Parse the response
    int responseId = 0;
    std::memcpy(&responseId, buffer + 4, sizeof(responseId));

    if (responseId != requestId)
        return false;

    return true;
}

bool RCON::sendConsoleCommand(const std::string& command, std::string* response) {

    Packet consolePacket;
    consolePacket.set(PACKET_TYPE_COMMAND, PACKET_TYPE_COMMAND, command);

    const std::string& serializedPacket = consolePacket.serialize();
    if (send(this->socketHandle, serializedPacket.c_str(), static_cast<int>(serializedPacket.size()), 0) < 0) {
        printf("[RCON -> SendConsoleCommand] Failed to send console command\n");
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        return false;
    }

    char* packetBuffer = nullptr;
    int packetBufSize = 0;

    while (true) {

        char tempBuffer[1024] = { 0 };
        int bytesRead = recv(this->socketHandle, tempBuffer, 1024, 0);

        if (bytesRead == -1) {
            delete[] packetBuffer;
            return -1;
        }

        // Allocate new buffer with updated size
        char* newBuf = new char[packetBufSize + bytesRead];

        // Copy the old data into the new buffer
        if (packetBuffer != nullptr)
            memcpy(newBuf, packetBuffer, packetBufSize);

        // Copy the new data into the new buffer
        memcpy(newBuf + packetBufSize, tempBuffer, bytesRead);

        // Update the packet buffer pointer
        delete[] packetBuffer;  // Free old memory
        packetBuffer = newBuf;   // Point to the new buffer

        // Update the buffer size
        packetBufSize += bytesRead;

        // If less than 1024 bytes were read, we're done
        if (bytesRead != 1024)
            break;
    }

    Packet responsePacket;
    responsePacket.deserialize(packetBuffer, static_cast<int>(packetBufSize));

    delete[] packetBuffer;

    *response = responsePacket.payload;

    return true;
}