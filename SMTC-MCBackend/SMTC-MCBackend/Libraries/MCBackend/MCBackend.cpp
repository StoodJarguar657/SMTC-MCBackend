#include "pch.h"
#include <MCBackend.hpp>
#include <filesystem>
#include <json.hpp>
#include <RCON.hpp>

class Packet {
private:
    std::vector<uint8_t> data;

public:
    uint8_t packetId;

    void addVarInt(int32_t value) {
        do {
            uint8_t temp = value & 0b01111111;
            value >>= 7;
            if (value != 0)
                temp |= 0b10000000;
            this->data.push_back(temp);
        } while (value != 0);
    }

    void addString(const std::string& str) {
        addVarInt(static_cast<int32_t>(str.length())); // VarInt length
        this->data.insert(this->data.end(), str.begin(), str.end()); // Raw UTF-8
    }

    void addUShort(uint16_t value) {
        this->data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF)); // High byte
        this->data.push_back(static_cast<uint8_t>(value & 0xFF));        // Low byte
    }

    void addByte(uint8_t byte) {
        this->data.push_back(byte);
    }

    void setPacketId(uint8_t id) {
        this->packetId = id;
    }

    std::vector<uint8_t> build() {
        std::vector<uint8_t> finalPacket;

        std::vector<uint8_t> packetData;
        packetData.push_back(this->packetId); // Packet ID (usually VarInt but mostly fits in 1 byte)
        packetData.insert(packetData.end(), this->data.begin(), this->data.end());

        // Prepend full packet length as VarInt
        std::vector<uint8_t> lengthPrefix;
        int32_t length = static_cast<int32_t>(packetData.size());
        do {
            uint8_t temp = length & 0b01111111;
            length >>= 7;
            if (length != 0)
                temp |= 0b10000000;
            lengthPrefix.push_back(temp);
        } while (length != 0);

        finalPacket.insert(finalPacket.end(), lengthPrefix.begin(), lengthPrefix.end());
        finalPacket.insert(finalPacket.end(), packetData.begin(), packetData.end());

        return finalPacket;
    }

    const uint8_t* getData() {
        return build().data();
    }

    size_t getSize() {
        return build().size();
    }
};
class Socket {
private:
    int socketHandle;

public:

    bool valid = false;

    Socket(int socketHandle = 0)
        : socketHandle(socketHandle) {
        if (this->socketHandle == 0)
            this->socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->socketHandle >= 0) {
            u_long mode = 0; // Blocking mode
            ioctlsocket(this->socketHandle, FIONBIO, &mode);
            this->valid = true;
        }

        return;
    }

    bool connectToAddr(const std::string& ip, uint32_t port) {
        sockaddr_in connection = {};
        connection.sin_family = AF_INET;
        connection.sin_port = htons(port);
        connection.sin_addr.s_addr = inet_addr(ip.c_str());
    
        if (connect(this->socketHandle, reinterpret_cast<sockaddr*>(&connection), sizeof(connection)) != 0)
            return false;
    
        return true;
    }
    bool bindToAddr(const std::string& ip, uint32_t port) {
        sockaddr_in connection = {};
        connection.sin_family = AF_INET;
        connection.sin_port = htons(port);
        connection.sin_addr.s_addr = inet_addr(ip.c_str());

        if (bind(this->socketHandle, reinterpret_cast<sockaddr*>(&connection), sizeof(connection)) < 0)
            return false;

        return true;
    }

    bool listenForConnection() {
        if (listen(this->socketHandle, 5) >= 0)
            return true;
        return false;
    }
    Socket acceptConnection() {
        sockaddr_in clientAddress = {};
        socklen_t clientLen = sizeof(clientAddress);
        return Socket(accept(this->socketHandle, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientLen));
    }

    int recieveBytes(char** packetBuffer) {

        int packetBufSize = 0;

        while (true) {

            char tempBuffer[1024] = { 0 };
            int bytesRead = recv(this->socketHandle, tempBuffer, 1024, 0);

            if (bytesRead == -1) {
                delete[] *packetBuffer;
                return -1;
            }
            
            // Allocate new buffer with updated size
            char* newBuf = new char[packetBufSize + bytesRead];

            // Copy the old data into the new buffer
            if (*packetBuffer != nullptr)
                memcpy(newBuf, *packetBuffer, packetBufSize);

            // Copy the new data into the new buffer
            memcpy(newBuf + packetBufSize, tempBuffer, bytesRead);

            // Update the packet buffer pointer
            delete[] * packetBuffer;  // Free old memory
            *packetBuffer = newBuf;   // Point to the new buffer

            // Update the buffer size
            packetBufSize += bytesRead;

            // If less than 1024 bytes were read, we're done
            if (bytesRead != 1024)
                break;
        }

        return packetBufSize;
    }
    void sendPacket(Packet* packet) {
        const std::vector<uint8_t>& bytes = packet->build();
        send(this->socketHandle, reinterpret_cast<const char*>(bytes.data()), bytes.size(), 0);
    }

    void closeConnection() {
        shutdown(this->socketHandle, SD_SEND);
        closesocket(this->socketHandle);
        this->socketHandle = 0;
    }
};

bool MCServerDesc::checkData() {
    
    if (!std::filesystem::exists(this->serverFolder)) {
        printf("[MCServerDesc -> checkData] serverFolder doesnt exist %s\n", this->serverFolder.string().c_str());
        return false;
    }

    if (!std::filesystem::exists(this->serverFolder / "server.properties")) {
        printf("[MCServerDesc -> checkData] server.properties doesnt exist, run the server once for them to generate\n");
        return false;
    }

    return true;
}

bool MCServer::initWithFolder(const std::filesystem::path& serverFolder) {

    this->folder = serverFolder;
    if (!std::filesystem::exists(this->folder / "server.properties")) {
        printf("[MCServer -> initWithFolder] Failed to find 'server.properties' in directory\n");
        return false;
    }

    // It is to be expected that the server has a "start.bat" or "start.sh"
    #ifdef _WIN32
        if (!std::filesystem::exists(serverFolder / "start.bat")) {
            printf("[MCServer -> initWithFolder] Failed to find start.bat\n");
            return false;
        }
        this->startFile = serverFolder / "start.bat";
    #else
        if (!std::filesystem::exists(serverFolder / "start.sh")) {
            printf("[MCServer -> initWithFolder] Failed to find start.sh\n");
            return false;
        }
        this->startFile = serverFolder / "start.sh";
    #endif

    std::ifstream properties(this->folder / "server.properties");
    if (!properties.is_open()) {
        printf("[MCServer -> initWithFolder] Failed to open server.properties\n");
        return false;
    }
    std::stringstream ss;
    ss << properties.rdbuf();
    properties.close();

    std::string line;
    while (std::getline(ss, line)) {

        // Check rcon enabled state
        if (line.find("enable-rcon") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);
            if (value != "true")
                printf("[MCServer -> initWithFolder] rcon is not enabled on server @ %s\n", serverFolder.string().c_str());
            continue;
        }

        // Check rcon port state
        if (line.find("rcon.port") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);
            this->rconPort = std::stoi(value);
            continue;
        }

        // Check server ip
        if (line.find("server-ip") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);

            if (value.empty()) {
                this->rawAddress = "0.0.0.0";
                this->address = "127.0.0.1";
                continue;
            }

            if (value == "0.0.0.0") {
                this->rawAddress = "0.0.0.0";
                this->address = "127.0.0.1";
                continue;
            }

            this->rawAddress = value;
            this->address = value;

            continue;
        }

        // Check server port -> if "" : 25565
        if (line.find("server-port") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);
            this->port = std::stoi(value);
            continue;
        }
    }

    // Check that all keys are filled, all of these should never happen
    // Either predefined or get set if empty
    if (this->rconPort == 0) {
        printf("[MCServer -> initWithFolder] rcon.port is undefined\n");
        return false;
    }
    if (this->address.empty()) {
        printf("[MCServer -> initWithFolder] server-ip is undefined\n");
        return false;
    }
    if (this->port == 0) {
        printf("[MCServer -> initWithFolder] server-port is undefined\n");
        return false;
    }

    return true;
}
bool MCServer::getStatus() {

    Socket socket;
    
    if (!socket.connectToAddr(this->address, this->port)) {

        if (this->state != SERVER_STATE::STARTING)
            this->state = SERVER_STATE::OFFLINE;

        socket.closeConnection();
        return false;
    }
    
    Packet handshake;
    handshake.setPacketId(0);
    handshake.addVarInt(0);
    handshake.addString(this->address);
    handshake.addUShort(this->port);
    handshake.addVarInt(1);
    socket.sendPacket(&handshake);
    
    Packet statusRequest;
    statusRequest.setPacketId(0x00);
    socket.sendPacket(&statusRequest);
    
    char* buffer = nullptr;
    int bytesRecieved = socket.recieveBytes(&buffer);
    if (bytesRecieved == -1) {
        socket.closeConnection();
        return false;
    }
    
    socket.closeConnection();
    
    const nlohmann::json& json = nlohmann::json::parse(std::string(buffer + 5, bytesRecieved - 5));
    if (json.empty()) {
        delete[] buffer;
        return false;
    }
    
    delete[] buffer;
    
    const auto& players = json.find("players");
    if (players == json.end() || !players->is_object())
        return false;
    
    const auto& online = players->find("online");
    if (online == players->end() || !online->is_number_integer())
        return false;
    
    uint32_t newPlayerCount = online->get<int>();
    if (this->playerCount != 0 && newPlayerCount == 0)
        this->lastPlayerActivity = time(nullptr);
    this->playerCount = newPlayerCount;
    
    if (this->playerCount > 0)
        this->state = SERVER_STATE::ONLINE_USING;
    else
        this->state = SERVER_STATE::ONLINE_EMPTY;
    
    return true;
}
bool MCServer::start() {

    #ifdef _WIN32
    STARTUPINFOA startupInfo = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION processInfo;
    if (CreateProcessA(
        this->startFile.string().c_str(),
        NULL, NULL, NULL,
        FALSE, 0, NULL,
        this->folder.string().c_str(),
        &startupInfo,
        &processInfo)) {
        this->state = SERVER_STATE::STARTING;
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        return true;
    }
    return false;
    #else

    const std::string filePath = this->startFile.string();

    // Ensure the file exists and is executable
    if (access(filePath.c_str(), X_OK) != 0) {
        perror("[MCServer -> start] File not executable or not found");
        return false;
    }

    pid_t processId = fork();
    if (processId == 0) {
        if (chdir(this->startFile.parent_path().string().c_str()) != 0) {
            perror("[MCServer -> start] Failed to change working directory");
            exit(1);
        }

        char* args[] = { const_cast<char*>(filePath.c_str()), NULL };
        
        if (execvp(args[0], args) == -1) {
            perror("[MCServer -> start] Failed to start server");
            return false;
        }
    } else if (processId > 0) {
        this->state = SERVER_STATE::STARTING;
        return true;
    } else {
        return false;
    }
    #endif
}
bool MCServer::stop() {

    this->state = SERVER_STATE::STOPPING;

    std::ifstream properties(this->folder / "server.properties");
    if (!properties.is_open()) {
        printf("[MCServer -> stop] Failed to open server.properties\n");
        return false;
    }
    std::stringstream ss;
    ss << properties.rdbuf();
    properties.close();

    std::string line;
    while (std::getline(ss, line)) {

        // Check rcon enabled state
        if (line.find("rcon.password") == std::string::npos)
            continue;

        const std::string& value = line.substr(line.find("=") + 1);

        if (!this->rcon.init(this->address, this->rconPort)) {
            printf("[MCServer -> stop] Failed to initialize rcon\n");
            return false;
        }

        if (!this->rcon.authenticate(value)) {
            printf("[MCServer -> stop] Failed to authenticate rcon\n");
            return false;
        }

        break;
    }

    std::string response;
    if (!this->rcon.sendConsoleCommand("stop", &response)) {
        printf("[MCServer -> stop] Failed to send stop command\n");
        return false;
    }

    return true;
}
bool MCServer::tpcListener() {

    Socket socket;
    if (!socket.bindToAddr(this->rawAddress, this->port)) {
        socket.closeConnection();
        return false;
    }

    if (!socket.listenForConnection()) {
        socket.closeConnection();
        return false;
    }

    Socket clientSocket = socket.acceptConnection();
    if (!clientSocket.valid) {
        clientSocket.closeConnection();
        return false;
    }

    char* buffer = nullptr;
    int recievedBytes = clientSocket.recieveBytes(&buffer);
    if (recievedBytes == -1) {
        clientSocket.closeConnection();
        socket.closeConnection();
        return false;
    }

    uint8_t nextState = buffer[7 + buffer[4]];
    delete[] buffer;

    Packet packet;
    packet.setPacketId(0x00);
    switch (nextState) {
        case 1: { // Status
            const std::string& json = R"({"version":{"name":"0","protocol":0},"players":{"max":0,"online":0},"description":{"text":")" + this->name + R"( [OFFLINE]"}})";;
            packet.addString(json);
            clientSocket.sendPacket(&packet);

            // Even tho i set the socket to block, it sometimes causes a error in Minecraft
            // Connection closed before any message arrived
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            
            clientSocket.closeConnection();
            socket.closeConnection();
            return false;
        }
        case 2: { // Login
            const std::string& json = R"({"text":"Server will start","color":"green"})";
            packet.addString(json);
            clientSocket.sendPacket(&packet);

            // Even tho i set the socket to block, it sometimes causes a error in Minecraft
            // Connection closed before any message arrived
            std::this_thread::sleep_for(std::chrono::milliseconds(25));

            clientSocket.closeConnection();
            socket.closeConnection();

            return true;
        }
    }

    clientSocket.closeConnection();
    socket.closeConnection();

    return false;
}
void MCServer::update() {

    while (true) {

        if (this->state == SERVER_STATE::OFFLINE) {
        
            this->lastPlayerActivity = time(nullptr);

            if (this->initDesc.startLogic == SERVER_DESC_START::KEEP_ONLINE) {
                this->start();
                continue;
            }

            if (this->initDesc.startLogic == SERVER_DESC_START::PROXY_SERVER) {
                // Have a tpc listener on the same address and port
                // If a player tries to connect, kick and start server
                if (this->tpcListener())
                    this->start();
                continue;
            }
        }
        
        this->getStatus();

        // Auto shutdown logic
        if (this->state != SERVER_STATE::ONLINE_EMPTY)
            continue;

        if (this->initDesc.stopLogic == SERVER_DESC_STOP::AUTO_STOP) {
            int timeNoPlayers = time(nullptr) - this->lastPlayerActivity;
            if (timeNoPlayers < this->initDesc.autoStopAfter)
                continue;
            this->stop();
        }
    }
}

void MCBackend::webServerThread(int webServerThread) {
    this->webServer.port(webServerThread).run();
    this->wantsShutdown = true; // This is only true if the user CTRL+C's into the console
}
MCBackend::MCBackend() {

    CROW_ROUTE(this->webServer, "/rconSend").methods("POST"_method) ([&](const crow::request& req) {
        return this->handleRCON(req);
    });

    CROW_ROUTE(this->webServer, "/readFile").methods("POST"_method) ([&](const crow::request& req) {
        return this->handleReadFile(req);
    });

}
bool MCBackend::addServer(MCServerDesc* serverDesc) {

    if (!serverDesc->checkData())
        return false;

    MCServer server;
    if (!server.initWithFolder(serverDesc->serverFolder))
        return false;

    // Check that the server doesnt interfere with others
    const std::string& serverName = serverDesc->serverFolder.filename().string();
    if (serverName.empty())
        server.name = serverDesc->serverFolder.parent_path().filename().string();
    else 
        server.name = serverName;

    for (const auto& [serverName, serverInstance] : this->servers) {

        if (server.folder == serverInstance.second.folder)
            continue;

        if (server.port == serverInstance.second.port) {
            printf("[MCBackend -> addServer] %s's port is identical to %s's\n", serverName.c_str(), serverInstance.second.folder.filename().string().c_str());
            return false;
        }
        if (server.rconPort == serverInstance.second.rconPort) {
            printf("[MCBackend -> addServer] %s's rcon port is identical to %s's\n", serverName.c_str(), serverInstance.second.folder.filename().string().c_str());
            return false;
        }
    }

    server.initDesc = *serverDesc;

    // Launch thread for the update function
    std::thread t(&MCServer::update, server);
    t.detach();

    this->servers.emplace(serverName, std::make_pair(std::move(t), std::move(server)));

    return true;
}
bool MCBackend::initialize(int webServerThread) {

    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;
    #endif

    std::thread webServer(&MCBackend::webServerThread, this, webServerThread);
    webServer.detach();

    return true;
}
bool MCBackend::deInitialize() {

    this->webServer.stop();
    this->wantsShutdown = true;

    #ifdef _WIN32
    WSACleanup();
    #endif

    return true;
}

void MCBackend::createCommand(const char* name, std::function<std::string(const crow::request& req)> fn) {
    this->commands[name] = fn;
}
std::string MCBackend::handleRCON(const crow::request& req) {

    time_t curTime = time(nullptr);
    if ((curTime - this->nextMessageSendable) < 0) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 1;
        returnValue["message"] = "Wait a few seconds before the next message.";
        return returnValue.dump();
    }
    this->nextMessageSendable = curTime + 1; // Set to 1 second
    
        
    // "Authorization" should have the RCON password
    if (!req.headers.contains("Authorization")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 2;
        returnValue["message"] = "Failed to find 'Authorization' in the header.";
        return returnValue.dump();
    }
    const auto& authorization = req.headers.find("Authorization");

    const nlohmann::json& body = nlohmann::json::parse(req.body, nullptr, false, true);
    
    // Check server to execute for
    if (!body.contains("serverName")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "Failed to find 'serverName' in the body.";
        return returnValue.dump();
    }
    const auto& targetServer = body.find("serverName");

    if (targetServer.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "serverName is not a string.";
        return returnValue.dump();
    }
    const std::string& targetServerName = targetServer.value().get<std::string>();

    if (!body.contains("message")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 5;
        returnValue["message"] = "Failed to find 'message' in the body.";
        return returnValue.dump();
    }
    const auto& jsonMessage = body.find("message");

    // Check types of payload
    if (jsonMessage.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 6;
        returnValue["message"] = "Message is not a string.";
        return returnValue.dump();
    }

    const auto& serverListBeg = this->servers.begin();
    const auto& serverListEnd = this->servers.end();

    MCServer* foundServer = nullptr;
    for (auto& [serverName, serverInstance] : this->servers) {
        if (serverName != targetServerName)
            continue;
        foundServer = &serverInstance.second;
        break;
    }

    if (foundServer == nullptr) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 7;
        returnValue["message"] = "Failed to find server.";
        return returnValue.dump();
    }

    const std::string& rconPassword = authorization->second;
    const std::string& payload = jsonMessage->get<std::string>();

    if (!foundServer->rcon.init(foundServer->address, foundServer->rconPort)) {
        crow::json::wvalue returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 8;
        returnValue["message"] = "Failed to initialize RCON (server offline).";
        this->nextMessageSendable = curTime + 10; // Block for 10 seconds
        return returnValue.dump();
    }

    if (!foundServer->rcon.authenticate(rconPassword)) {
        crow::json::wvalue returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 9;
        returnValue["message"] = "RCON password is invalid or server unreachable.";
        this->nextMessageSendable = curTime + 10; // Block for 10 seconds
        return returnValue.dump();
    }

    const std::string& commandName = payload.substr(0 ,payload.find_first_of(" "));
    const auto& customCommand = this->commands.find(commandName);
    if (customCommand == this->commands.end()) {
        std::string commandResponse;
        if (!foundServer->rcon.sendConsoleCommand(payload, &commandResponse)) {
            crow::json::wvalue returnValue;
            returnValue["status"] = "failed";
            returnValue["errorCode"] = 10;
            returnValue["message"] = "Command is invalid.";
            return returnValue.dump();
        }
        
        crow::json::wvalue returnValue;
        returnValue["status"] = "success";
        returnValue["message"] = commandResponse;
        return returnValue.dump();
    }

    // Its a custom command
    return customCommand->second(req);
}
std::string MCBackend::handleReadFile(const crow::request& req) {

    const nlohmann::json& body = nlohmann::json::parse(req.body, nullptr, false, true);
    if (!body.contains("filePath")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 1;
        returnValue["message"] = "Failed to find 'filePath' in the body.";
        return returnValue.dump();
    }
    const auto& targetFilePath = body.find("filePath");

    if (targetFilePath.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 2;
        returnValue["message"] = "Filepath is not a string.";
        return returnValue.dump();
    }

    const std::filesystem::path& filePath = targetFilePath->get<std::filesystem::path>();
    if (!filePath.is_relative()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "Filepath must be relative.";
        return returnValue.dump();
    }

    if (filePath.string().find("..") != std::string::npos) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "Cannot enter parent directory.";
        return returnValue.dump();
    }

    if (!body.contains("serverName")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 5;
        returnValue["message"] = "Failed to find 'serverName' in the body.";
        return returnValue.dump();
    }

    const auto& targetServer = body.find("serverName");

    if (targetServer.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 6;
        returnValue["message"] = "serverName is not a string.";
        return returnValue.dump();
    }

    const std::string& targetServerName = targetServer.value().get<std::string>();

    const auto& serverListBeg = this->servers.begin();
    const auto& serverListEnd = this->servers.end();

    MCServer* foundServer = nullptr;
    for (auto& [serverName, serverInstance] : this->servers) {
        if (serverName != targetServerName)
            continue;
        foundServer = &serverInstance.second;
        break;
    }

    if (foundServer == nullptr) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 7;
        returnValue["message"] = "Failed to find server.";
        return returnValue.dump();
    }

    const std::filesystem::path& absPath = foundServer->folder / filePath;

    if (!std::filesystem::exists(absPath)) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 8;
        returnValue["message"] = "File doesnt exist.";
        return returnValue.dump();
    }

    std::ifstream file(absPath);
    if (!file.is_open()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 9;
        returnValue["message"] = "Failed to open file.";
        return returnValue.dump();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    nlohmann::json returnValue;
    returnValue["status"] = "success";
    returnValue["message"] = ss.str();

    return returnValue.dump();
}
