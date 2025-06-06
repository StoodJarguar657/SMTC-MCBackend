#ifndef RCON_HPP
#define RCON_HPP

#include <string>
#include <unordered_map>

class RCON {
private:

    const int PACKET_TYPE_AUTH = 3;
    const int PACKET_TYPE_COMMAND = 2;

    int socketHandle = 0;

    std::unordered_map<int, std::string> responses = {};

    std::string ip;
    int port;

public:

    struct Packet {
        int length;
        int requestId;
        int type;
        std::string payload;

        void set(int reqId, int packetType, const std::string& data);

        std::string serialize();

        void deserialize(const char* buffer, int bytesRead);
    };

    bool init(const std::string& ip, int port = 25575);

    bool authenticate(const std::string& password);

    bool sendConsoleCommand(const std::string& command, std::string* response);

};

#endif // RCON_HPP