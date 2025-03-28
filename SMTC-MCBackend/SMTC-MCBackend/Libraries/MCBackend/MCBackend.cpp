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

    static std::vector<uint8_t> packet = { 0xFE, 0x01, 0xFA, 0x00, 0x0B, 0x00, 0x4D, 0x00, 0x43, 0x00, 0x7C, 0x00, 0x50, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x48, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x74 };

    // Make the packet once
    if (packet.size() == 27) {

        size_t addressLength = this->address.size();

        short hostNameLength = 7 + addressLength * 2;
        packet.push_back((hostNameLength >> 8) & 0xFF);
        packet.push_back(hostNameLength & 0xFF);

        packet.push_back(0x49);

        packet.push_back(0x00);
        packet.push_back(addressLength);

        for (int i = 0; i < addressLength; i++) {
            packet.push_back(0x00);
            packet.push_back(this->address[i]);
        }

        packet.push_back((this->port >> 24) & 0xFF);
        packet.push_back((this->port >> 16) & 0xFF);
        packet.push_back((this->port >> 8) & 0xFF);
        packet.push_back(this->port & 0xFF);
    }

    this->socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->socketHandle == INVALID_SOCKET) {
        printf("[MCBackend -> getStatus] Failed to create socket\n");
        return false;
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = inet_addr(this->address.c_str());

    if (connect(this->socketHandle, reinterpret_cast<sockaddr*>(&server), sizeof(server)) == SOCKET_ERROR) {
        printf("[MCBackend -> getStatus] Failed to connect\n");
        closesocket(this->socketHandle);
        this->socketHandle = 0;

        if (this->state != SERVER_STATE::STARTING)
            this->state = SERVER_STATE::OFFLINE;

        return false;
    }

    if (!send(this->socketHandle, reinterpret_cast<const char*>(packet.data()), packet.size(), 0)) {
        printf("[MCBackend -> getStatus] Failed to send data\n");
        closesocket(this->socketHandle);
        this->socketHandle = 0;

        if (this->state != SERVER_STATE::STARTING)
            this->state = SERVER_STATE::OFFLINE;

        return false;
    }

    uint8_t buffer[4096];
    int bytesRecieved = recv(this->socketHandle, reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
    if (bytesRecieved == SOCKET_ERROR) {
        printf("[MCBackend -> getStatus] Failed to recieve data\n");
        closesocket(this->socketHandle);
        this->socketHandle = 0;

        if (this->state != SERVER_STATE::STARTING)
            this->state = SERVER_STATE::OFFLINE;

        return false;
    }

    auto readBuf = [&](size_t off) -> size_t {
        for (size_t i = off; i < bytesRecieved - 2; i += 2) {
            if (buffer[i] != 0x00 || buffer[i + 1] != 0x00)
                continue;
            return i + 2;
        }
        return 0;
    };

    size_t startPos = 0;
    size_t endPos = readBuf(startPos);
    size_t count = 0;

    while (endPos != 0) {
        count++;
        switch (count) {
            case 1: // Trash
                break;
            case 2: // Start of ip
                break;
            case 3: // Version
                break;
            case 4: // Motd
                break;
            case 5: { // Current players

                if (this->state == SERVER_STATE::STARTING) {
                    if (!this->rcon.init(this->address, this->rconPort)) {
                        printf("[MCServer -> getStatus] Failed to initialize rcon\n");
                        return false;
                    }
                }

                if (buffer[startPos] != 0x30) {
                    this->serverEmptyTime = 0;
                    this->state = SERVER_STATE::ONLINE_USING;
                    break;
                }

                    //*isEmpty = true;
                if (this->serverEmptyTime == 0) {
                    this->serverEmptyTime = time(nullptr);
                    this->state = SERVER_STATE::ONLINE_EMPTY;
                }

                if (this->initDesc.autoStop && (time(nullptr) - this->serverEmptyTime) > this->initDesc.autoStopAfter)
                    this->stop();

                break;
            }
        }

        startPos = endPos;
        endPos = readBuf(startPos);
    }

    closesocket(this->socketHandle);
    this->socketHandle = 0;

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
bool MCServer::stop() {

    std::ifstream properties(this->folder / "server.properties");
    std::stringstream ss;
    ss << properties.rdbuf();
    properties.close();

    std::string line;
    while (std::getline(ss, line)) {

        // Check rcon enabled state
        if (line.find("rcon.password") != std::string::npos) {
            const std::string& value = line.substr(line.find("=") + 1);

            if (!this->rcon.authenticate(value))
                return false;

            break;
        }
    }

    std::string response;
    this->rcon.sendConsoleCommand("/stop", &response);

    return true;
}

void MCServer::tpcListener() {
    
    if (!this->socketHandle) {
        printf("[MCBackend -> tpcListener] Started listening on %s:%d\n", this->address.c_str(), this->port);
    
        this->socketHandle = socket(AF_INET, SOCK_STREAM, 0);
        if (this->socketHandle == INVALID_SOCKET) {
            printf("[MCBackend -> tpcListener] Failed to create socket for %s:%d\n", this->address.c_str(), this->port);
            return;
        }
    
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(this->port);
        if (inet_pton(AF_INET, this->address.c_str(), &serverAddr.sin_addr) <= 0) {
            printf("[MCServer -> tpcListener] Invalid address: %s\n", this->address.c_str());
            closesocket(this->socketHandle);
            this->socketHandle = 0;
            return;
        }
    
        int opt = 1;
        setsockopt(this->socketHandle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

        struct linger so_linger;
        so_linger.l_onoff = 1;  // Enable linger option
        so_linger.l_linger = 1; // Allow 1 second for cleanup
        setsockopt(this->socketHandle, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&so_linger), sizeof(so_linger));
    
        if (bind(this->socketHandle, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            printf("[MCServer -> tpcListener] Failed to bind %s:%d\n", this->address.c_str(), this->port);
            closesocket(this->socketHandle);
            this->socketHandle = 0;
            return;
        }
    
        if (listen(this->socketHandle, 5) < 0) {
            printf("[MCServer -> tpcListener] Failed to listen on %s:%d\n", this->address.c_str(), this->port);
            closesocket(this->socketHandle);
            this->socketHandle = 0;
            return;
        }
    
        u_long mode = 1;  // 1 = non-blocking
        ioctlsocket(this->socketHandle, FIONBIO, &mode);
    
        this->listeningActive = true;
    }
    
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(this->socketHandle, &readSet);
    
    int activity = select(0, &readSet, NULL, NULL, &timeout);
    if (activity == SOCKET_ERROR) {
        printf("[MCServer -> tpcListener] Failed to select\n");
        closesocket(this->socketHandle);
        this->socketHandle = 0;
        return;
    }
    
    if (activity <= 0 || !FD_ISSET(this->socketHandle, &readSet))
        return;
    
    u_long mode = 0;  // 1 = non-blocking
    ioctlsocket(this->socketHandle, FIONBIO, &mode);
    
    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);
    int clientSocket = accept(this->socketHandle, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientLength);
    
    // https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2 -> 10035
    // This error is returned from operations on nonblocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
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
    if (recv(clientSocket, buffer, 4096, 0) == -1) {
        closesocket(clientSocket);
        return;
    }
    
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
            break;
        }
        case 2: { // Login
            // This happens when the user is trying to join the server
            // Send "Server will start" back since server is offline rn <- cant figure this out
    
            closesocket(this->socketHandle);
            this->socketHandle = 0;
            this->listeningActive = false;
            printf("[MCBackend -> tpcListener] Started server on %s:%d\n", this->address.c_str(), this->port);
    
            if (!this->start())
                break;

            break;
        }
        case 3: { // Transfer
            break;
        }
    }
    
    closesocket(clientSocket);

}

void MCBackend::webServerThread(int webServerThread) {
    this->webServer.port(webServerThread).run();
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

        if (server.port == otherServer.port) {
            printf("[MCBackend -> addServer] %s's port is identical to %s's\n", serverName.c_str(), otherServer.folder.filename().string().c_str());
            return false;
        }
        if (server.rconPort == otherServer.rconPort) {
            printf("[MCBackend -> addServer] %s's rcon port is identical to %s's\n", serverName.c_str(), otherServer.folder.filename().string().c_str());
            return false;
        }
    }

    server.initDesc = serverDesc;

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

    for (auto& server : this->servers) {

        if (server.listeningActive) {
            server.tpcListener();
            continue;
        }

        if (!server.getStatus()) {
            if (server.state == SERVER_STATE::OFFLINE)
                server.tpcListener();
        }
    }
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