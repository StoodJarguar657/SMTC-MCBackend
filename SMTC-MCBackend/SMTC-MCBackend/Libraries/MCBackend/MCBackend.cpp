#include "MCBackend.hpp"
#include <filesystem>
#include <json.hpp>
#include <RCON.hpp>

bool MCServerDesc::checkData() {
    
    if (!std::filesystem::exists(this->serverFolder)) {
        printf("[MCServerDesc -> checkData] serverFolder doesnt exist\n");
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

        // Check server ip -> if "" : localhost
        if (line.find("server-ip") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);
            if (value.empty())
                this->address = "localhost";
            else
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
    //static std::vector<uint8_t> packet = { 0xFE, 0x01, 0xFA, 0x00, 0x0B, 0x00, 0x4D, 0x00, 0x43, 0x00, 0x7C, 0x00, 0x50, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x48, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x74 };
    //
    //// Make the packet once
    //if (packet.size() == 27) {
    //    
    //    short hostNameLength = 7 + strlen(this->initDesc.serverAddress) * 2;
    //    packet.push_back((hostNameLength >> 8) & 0xFF);
    //    packet.push_back(hostNameLength & 0xFF);
    //    
    //    packet.push_back(0x49);
    //    
    //    packet.push_back(0x00);
    //    packet.push_back(strlen(this->initDesc.serverAddress));
    //    
    //    size_t serverAddrLen = strlen(this->initDesc.serverAddress);
    //    for (int i = 0; i < serverAddrLen; i++) {
    //        packet.push_back(0x00);
    //        packet.push_back(this->initDesc.serverAddress[i]);
    //    }
    //    
    //    packet.push_back((this->initDesc.serverPort >> 24) & 0xFF);
    //    packet.push_back((this->initDesc.serverPort >> 16) & 0xFF);
    //    packet.push_back((this->initDesc.serverPort >> 8) & 0xFF);
    //    packet.push_back(this->initDesc.serverPort & 0xFF);
    //}
    //
    //SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //if (sock == INVALID_SOCKET) {
    //    printf("[MCBackend -> checkServerStatus] Failed to create socket\n");
    //    return false;
    //}
    //
    //sockaddr_in server;
    //memset(&server, 0, sizeof(server));
    //server.sin_family = AF_INET;
    //server.sin_port = htons(this->initDesc.serverPort);
    //server.sin_addr.s_addr = inet_addr(this->initDesc.serverAddress);
    //
    //if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
    //    printf("[MCBackend -> checkServerStatus] Failed to connect\n");
    //    return false;
    //}
    //
    //if (!send(sock, reinterpret_cast<const char*>(packet.data()), packet.size(), 0) ) {
    //    printf("[MCBackend -> checkServerStatus] Failed to send data\n");
    //    return false;
    //}
    //
    //uint8_t buffer[4096];
    //int bytesReceived = recv(sock, reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
    //if (bytesReceived == SOCKET_ERROR) {
    //    printf("[MCBackend -> checkServerStatus] Failed to recieve data\n");
    //    return false;
    //}
    //
    //// Split at null terminators (splitters)
    //// The 4th is always the player count
    //int nullTerminators = 0;
    //for (size_t i = 0; i < bytesReceived - 1; ++i) {
    //    if (buffer[i] == 0x00 && buffer[i + 1] == 0x00)
    //        nullTerminators++;
    //
    //    if (nullTerminators == 4) {
    //        printf("");
    //    }
    //}
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
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        return true;
    }
    return false;
#else
    pid_t processId = fork();
    if (processId == 0) {
        char* args[] = { this->startFile.string().c_str(), NULL };
        execvp(args[0], args);
        return false;
    } else if (processId > 0) {
        return true;
    } else {
        return false;
    }
#endif
}

void MCBackend::webServerThread(int webServerThread) {
    this->webServer.port(webServerThread).run();
}

bool MCBackend::tpcListener() {

    printf("[MCBackend -> tpcListener] Starting listener\n");
    
    for (const MCServer& server : this->servers) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            printf("[MCBackend -> tpcListener] Failed to initialize socket\n");
            continue;
        }

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
        
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(25565);
    
        if (inet_pton(AF_INET, server.address.c_str(), &serverAddress.sin_addr) <= 0) {
            printf("[MCBackend -> tpcListener] Invalid IP address format\n");
            closesocket(serverSocket);
            continue;
        }

        int bindError = bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress));
        if (bindError < 0) {
            printf("[MCBackend -> tpcListener] Failed to bind socket\n");
            closesocket(serverSocket);
            continue;
        }

        if (listen(serverSocket, 5) < 0) {
            printf("[MCBackend -> tpcListener] Failed to listen\n");
            closesocket(serverSocket);
            continue;
        }

        while (true) {

            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(serverSocket, &fds);

            int selectResult = select(serverSocket + 1, &fds, NULL, NULL, &timeout);
            if (selectResult <= 0)
                continue; // Timeout / Error


            sockaddr_in clientAddress{};
            socklen_t clientLength = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientLength);

            // Packet structure
            // Length, id, protocol, data
            // 1b      1b  2b        xxb
            // 
            // Packet length   packet id   protocol    data
            // 10 -> 17           00         81 06   ip:port 1 == status
            // 10 00 81 06 09 31 32 37 2E 30 2E 30 2E 31 63 DD 01
            // 
            // 21 -> 34           00         81 06   kubernetes.docker.internal port 2 == login
            // 21 00 81 06 1A 6B 75 62 65 72 6E 65 74 65 73 2E 64 6F 63 6B 65 72 2E 69 6E 74 65 72 6E 61 6C 63 DD 02

            char buffer[4096] = { 0 };
            recv(clientSocket, buffer, 4096, 0);

            // Byte0 -> length (xx)
            // Byte1 -> packetId (00)
            // Byte2-3 -> protocol
            uint16_t protocol = 0;
            memcpy(&protocol, buffer + 2, 2);

            uint8_t stringLength = 0;
            memcpy(&stringLength, buffer + 4, 1);
            char textElement[130] = { 0 };
            memcpy(&textElement, buffer + 5, stringLength);

            uint8_t port1 = 0;
            uint8_t port2 = 0;
            memcpy(&port1, buffer + 5 + stringLength, 1);
            memcpy(&port2, buffer + 6 + stringLength, 1);
            uint16_t port = (port1 << 8) | port2;

            uint8_t nextState = 0;
            memcpy(&nextState, buffer + 7 + stringLength, 1);

            // TODO: for multi server support
            // Check "nextState" 
            switch (nextState) {
            case 1: { // Status
                // This happens when the server is in the server list
                // And the user presses refresh

                // Send Server state back (its defo offline rn)

                //{
                //    "version": {
                //        "name": "1.21.2",
                //            "protocol" : 768
                //    },
                //        "players" : {
                //        "max": 100,
                //            "online" : 5,
                //            "sample" : [
                //        {
                //            "name": "thinkofdeath",
                //                "id" : "4566e69f-c907-48ee-8d71-d7ba5aa00d20"
                //        }
                //            ]
                //    },
                //        "description": {
                //        "text": "Hello, world!"
                //    },
                //        "favicon" : "data:image/png;base64,<data>",
                //        "enforcesSecureChat" : false
                //}

                break;
            }
            case 2: { // Login
                // This happens when the user is trying to join the server
                // Send "Server will start" back since server is offline rn


                break;
            }
            case 3: { // Transfer
                break;
            }
            }


            if (clientSocket < 0) {
                printf("[MCBackend -> tpcListener] Failed to accept incomming client connection\n");
                continue;
            }

            const char* serverWillBeStarting = "\x21\x0\x1F\x7B\x22\x74\x65\x78\x74\x22\x3A\x20\x22\x53\x65\x72\x76\x65\x72\x20\x69\x73\x20\x73\x74\x61\x72\x74\x69\x6E\x67\x2E\x22\x7D";
            send(clientSocket, reinterpret_cast<const char*>(serverWillBeStarting), 35, 0);
            closesocket(clientSocket);
            break;
        }
    }

    return true;
}

MCBackend::MCBackend() {

    CROW_ROUTE(this->webServer, "/rconSend").methods("POST"_method) ([&](const crow::request& req) {
        return this->handleRCON(req);
    });

    CROW_ROUTE(this->webServer, "/readFile").methods("POST"_method) ([&](const crow::request& req) {
        return this->handeReadFile(req);
    });

}

bool MCBackend::addServer(MCServerDesc serverDesc) {

    if (!serverDesc.checkData())
        return false;

    // Check that the server doesnt interfere with others
    std::string serverName = serverDesc.serverFolder.filename().string();
    if (serverName.empty()) {
        serverDesc.serverFolder = serverDesc.serverFolder.parent_path();
        serverName = serverDesc.serverFolder.filename().string();
    }

    auto& server = this->servers.emplace_back();
    if (!server.initWithFolder(serverDesc.serverFolder)) {
        printf("[MCBackend -> addServer] Failed to initialize server\n");
        return false;
    }

    server.name = serverName;

    for (const auto& otherServer : this->servers) {
        if (server.folder == otherServer.folder)
            continue;

        if (server.address == otherServer.address) {
            printf("[MCBackend -> addServer] %s's address is identical to %s's\n", serverName.c_str(), otherServer.folder.filename().string().c_str());
            return false;
        }
        if (server.port == otherServer.port) {
            printf("[MCBackend -> addServer] %s's port is identical to %s's\n", serverName.c_str(), otherServer.folder.filename().string().c_str());
            return false;
        }
        if (server.rconPort == otherServer.rconPort) {
            printf("[MCBackend -> addServer] %s's rcon port is identical to %s's\n", serverName.c_str(), otherServer.folder.filename().string().c_str());
            return false;
        }
    }

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

#ifdef _WIN32
    WSACleanup();
#endif

    return true;
}

void MCBackend::update() {

    //// Check the server status
    //// If the rcon failed to init (most likely is the server offline)
    //if (!this->rcon.isInit) {
    //    if (!this->initDesc.autoStartServer)
    //        return;
    //
    //    // If it succeeds, this thread will freeze
    //    if (!this->tpcListener())
    //        return; // Some error occured
    //
    //    //// A client has pinged the ip:port
    //    //// Execute the start file once
    //    //time_t curTime = time(nullptr);
    //    //if (curTime - this->nextServerStart < 0)
    //    //    return; // Server is already starting
    //    //
    //    //this->startServer();
    //    //this->nextServerStart = curTime + 30; // Block server starting for 30sec
    //    //
    //    //while (true)
    //    //    if (this->rcon.init(this->initDesc.rconAddr, this->initDesc.rconPort))
    //    //        break;
    //
    //} else {
    //    //if (!this->initDesc.autoStopServer)
    //    //    return;
    //    //// Check player count, shut down
    //    //
    //    //bool isEmpty = false;
    //    //if (!checkServerStatus(&isEmpty))
    //    //    return;
    //
    //
    //}
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

    if (targetServer.value() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "serverName is not a string.";
        return returnValue.dump();
    }
    const std::string& targetServerName = targetServer.value().get<std::string>();

    const auto& serverListBeg = this->servers.begin();
    const auto& serverListEnd = this->servers.end();

    MCServer* foundServer = nullptr;
    for (auto& server : this->servers) {
        if (server.name != targetServerName)
            continue;
        foundServer = &server;
        break;
    }

    if (foundServer == nullptr) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 5;
        returnValue["message"] = "Failed to find server.";
        return returnValue.dump();
    }

    if (!body.contains("message")) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 6;
        returnValue["message"] = "Failed to find 'message' in the body.";
        return returnValue.dump();
    }
    const auto& jsonMessage = body.find("message");

    // Check types of payload
    if (jsonMessage.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 7;
        returnValue["message"] = "Message is not a string.";
        return returnValue.dump();
    }

    const std::string& rconPassword = authorization->second;
    const std::string& payload = jsonMessage->get<std::string>();

    if (!foundServer->rcon.authenticate(rconPassword)) {
        crow::json::wvalue returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 8;
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
            returnValue["errorCode"] = 9;
            returnValue["message"] = "Command is invalid.";
            return returnValue.dump().c_str();
        }
        
        crow::json::wvalue returnValue;
        returnValue["status"] = "success";
        returnValue["message"] = commandResponse;
        return returnValue.dump().c_str();
    }

    // Its a custom command
    return customCommand->second(req);
}
std::string MCBackend::handeReadFile(const crow::request& req) {

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
    
    if (targetFilePath.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "serverName is not a string.";
        return returnValue.dump();
    }

    const std::filesystem::path& filePath = targetFilePath->get<std::filesystem::path>();
    if (!filePath.is_relative()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "Filepath must be relative.";
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

    const std::string& targetServerName = targetServer.value().get<std::string>();

    const auto& serverListBeg = this->servers.begin();
    const auto& serverListEnd = this->servers.end();

    MCServer* foundServer = nullptr;
    for (auto& server : this->servers) {
        if (server.name != targetServerName)
            continue;
        foundServer = &server;
        break;
    }

    if (foundServer == nullptr) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 6;
        returnValue["message"] = "Failed to find server.";
        return returnValue.dump();
    }

    const std::filesystem::path& absPath = foundServer->folder / filePath;
    
    if (!std::filesystem::exists(absPath)) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 7;
        returnValue["message"] = "File doesnt exist.";
        return returnValue.dump();
    }
    
    std::ifstream file(absPath);
    if (!file.is_open()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 8;
        returnValue["message"] = "Failed to open file.";
        return returnValue.dump();
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
    return "";
    }