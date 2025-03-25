#include "MCBackend.hpp"
#include <filesystem>
#include <json.hpp>
#include <RCON.hpp>

MCBackend::MCBackend() {

    CROW_ROUTE(this->webServer, "/rconSend").methods("POST"_method) ([&](const crow::request& req) {
        return this->handleRCON(req);
    });

    CROW_ROUTE(this->webServer, "/readFile").methods("POST"_method) ([&](const crow::request& req) {
        return this->handeReadFile(req);
    });

}

void MCBackend::webServerThread() {
    this->webServer.port(this->initDesc.webServerPort).run();
}
bool MCBackend::tpcListener() {
        
    printf("[MCBackend -> tpcListener] Starting listener\n");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;
    
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        printf("[MCBackend -> tpcListener] Failed to initialize socket\n");
        return false;
    }
    
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(25565);
    
    if (inet_pton(AF_INET, this->initDesc.serverAddress, &serverAddress.sin_addr) <= 0) {
        printf("[MCBackend -> tpcListener] Invalid IP address format\n");
        closesocket(server);
        WSACleanup();
        return false;
    }

    int bindError = bind(server, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress));
    if (bindError < 0) {
        printf("[MCBackend -> tpcListener] Failed to bind socket\n");
        return false;
    }
    
    if (listen(server, 5) < 0) {
        printf("[MCBackend -> tpcListener] Failed to listen\n");
        return false;
    }
    
    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);
        int clientSocket = accept(server, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientLength);

        if (clientSocket < 0) {
            printf("[MCBackend -> tpcListener] Failed to accept incomming client connection\n");
            continue;
        }

        const char* serverWillBeStarting = "\x21\x0\x1F\x7B\x22\x74\x65\x78\x74\x22\x3A\x20\x22\x53\x65\x72\x76\x65\x72\x20\x69\x73\x20\x73\x74\x61\x72\x74\x69\x6E\x67\x2E\x22\x7D";
        send(clientSocket, reinterpret_cast<const char*>(serverWillBeStarting), 35, 0);
        closesocket(clientSocket);
        break;
    }

    closesocket(server);
    WSACleanup();

    return true;
}

bool MCBackend::startServer() {
#ifdef _WIN32
    STARTUPINFOA startupInfo = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION processInfo;
    if (CreateProcessA(
        this->startFile.string().c_str(),
        NULL, NULL, NULL,
        FALSE, 0, NULL,
        this->initDesc.serverFolder.string().c_str(),
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
    }
    else if (processId > 0) {
        return true;
    } else {
        return false;
    }
#endif
}

bool MCBackend::start(MCBackendDesc initDesc) {

    // It is to be expected that the server has a "start.bat" or "start.sh"
#ifdef _WIN32
    if (!std::filesystem::exists(initDesc.serverFolder / "start.bat")) {
        printf("[MCBacked -> start] Failed to find start.bat\n");
        return false;
    }
    this->startFile = initDesc.serverFolder / "start.bat";
#else
    if (!std::filesystem::exists(initDesc.serverFolder / "start.sh")) {
        printf("[MCBacked -> start] Failed to find start.sh\n");
        return false;
    }
    this->startFile = initDesc.serverFolder / "start.sh";
#endif

    this->initDesc = initDesc;
    if (!this->rcon.init(this->initDesc.rconAddr, this->initDesc.rconPort))
        printf("[MCBacked -> start] Failed to initialize rcon\n");
        
    this->webServer.loglevel(crow::LogLevel::Debug);

    std::thread serverThread(&MCBackend::webServerThread, this);
    serverThread.detach();

    return true;
}
bool MCBackend::checkServerStatus(bool* isEmpty) {
    static std::vector<uint8_t> packet = { 0xFE, 0x01, 0xFA, 0x00, 0x0B, 0x00, 0x4D, 0x00, 0x43, 0x00, 0x7C, 0x00, 0x50, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x48, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x74 };

    // Make the packet once
    if (packet.size() == 27) {
        
        short hostNameLength = 7 + strlen(this->initDesc.serverAddress) * 2;
        packet.push_back((hostNameLength >> 8) & 0xFF);
        packet.push_back(hostNameLength & 0xFF);
        
        packet.push_back(0x49);
        
        packet.push_back(0x00);
        packet.push_back(strlen(this->initDesc.serverAddress));
        
        size_t serverAddrLen = strlen(this->initDesc.serverAddress);
        for (int i = 0; i < serverAddrLen; i++) {
            packet.push_back(0x00);
            packet.push_back(this->initDesc.serverAddress[i]);
        }
        
        packet.push_back((this->initDesc.serverPort >> 24) & 0xFF);
        packet.push_back((this->initDesc.serverPort >> 16) & 0xFF);
        packet.push_back((this->initDesc.serverPort >> 8) & 0xFF);
        packet.push_back(this->initDesc.serverPort & 0xFF);
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("[MCBackend -> checkServerStatus] Failed to create socket\n");
        return false;
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(this->initDesc.serverPort);
    server.sin_addr.s_addr = inet_addr(this->initDesc.serverAddress);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[MCBackend -> checkServerStatus] Failed to connect\n");
        return false;
    }
    
    if (!send(sock, reinterpret_cast<const char*>(packet.data()), packet.size(), 0) ) {
        printf("[MCBackend -> checkServerStatus] Failed to send data\n");
        return false;
    }
    
    uint8_t buffer[4096];
    int bytesReceived = recv(sock, reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        printf("[MCBackend -> checkServerStatus] Failed to recieve data\n");
        return false;
    }

    // Split at null terminators (splitters)
    // The 4th is always the player count
    int nullTerminators = 0;
    for (size_t i = 0; i < bytesReceived - 1; ++i) {
        if (buffer[i] == 0x00 && buffer[i + 1] == 0x00)
            nullTerminators++;

        if (nullTerminators == 4) {
            printf("");
        }
    }

    return true;
}

void MCBackend::update() {

    // Check the server status
    // If the rcon failed to init (most likely is the server offline)
    if (!this->rcon.isInit) {
        if (!this->initDesc.autoStartServer)
            return;

        // If it succeeds, this thread will freeze
        if (!this->tpcListener())
            return; // Some error occured

        // A client has pinged the ip:port
        // Execute the start file once
        time_t curTime = time(nullptr);
        if (curTime - this->nextServerStart < 0)
            return; // Server is already starting

        this->startServer();
        this->nextServerStart = curTime + 30; // Block server starting for 30sec

        while (true)
            if (this->rcon.init(this->initDesc.rconAddr, this->initDesc.rconPort))
                break;

    } else {
        if (!this->initDesc.autoStopServer)
            return;
        // Check player count, shut down

        bool isEmpty = false;
        if (!checkServerStatus(&isEmpty))
            return;


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
    const auto& authorization = req.headers.find("Authorization");
    if (authorization == req.headers.end()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 2;
        returnValue["message"] = "Failed to find 'Authorization' in the header.";
        return returnValue.dump();
    }

    const nlohmann::json& body = nlohmann::json::parse(req.body, nullptr, false, true);

    const auto& jsonMessage = body.find("message");
    if (jsonMessage == body.end()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "Failed to find 'message' in the body.";
        return returnValue.dump();
    }

    // Check types of payload
    if (jsonMessage.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "Message is not a string.";
        return returnValue.dump();
    }

    const std::string& rconPassword = authorization->second;
    const std::string& payload = jsonMessage->get<std::string>();

    if (!this->rcon.authenticate(rconPassword)) {
        crow::json::wvalue returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 5;
        returnValue["message"] = "RCON password is invalid.";
        this->nextMessageSendable = curTime + 10; // Block for 10 seconds
        return returnValue.dump();
    }

    const std::string& commandName = payload.substr(0 ,payload.find_first_of(" "));
    const auto& customCommand = this->commands.find(commandName);
    if (customCommand == this->commands.end()) {
        std::string commandResponse;
        if (!rcon.sendConsoleCommand(payload, &commandResponse)) {
            crow::json::wvalue returnValue;
            returnValue["status"] = "failed";
            returnValue["errorCode"] = 6;
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
    const auto& jsonMessage = body.find("filePath");
    if (jsonMessage == body.end()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 1;
        returnValue["message"] = "Failed to find 'filePath' in the body.";
        return returnValue.dump();
    }

    if (jsonMessage.value().type() != nlohmann::json::value_t::string) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 2;
        returnValue["message"] = "Filepath is not a string.";
        return returnValue.dump();
    }

    const std::filesystem::path& filePath = jsonMessage->get<std::filesystem::path>();
    if (!filePath.is_relative()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "Filepath must be relative.";
        return returnValue.dump();
    }

    const std::filesystem::path& absPath = this->initDesc.serverFolder / filePath;

    if (!std::filesystem::exists(absPath)) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "File doesnt exist.";
        return returnValue.dump();
    }

    std::ifstream file(absPath);
    if (!file.is_open()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 5;
        returnValue["message"] = "Failed to open file.";
        return returnValue.dump();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}