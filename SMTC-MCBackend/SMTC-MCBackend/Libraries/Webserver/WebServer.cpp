#include "Webserver.hpp"
#include <filesystem>
#include <json.hpp>
#include <RCON.hpp>

WebServer::WebServer() :
    rcon("192.168.2.133") {

    CROW_ROUTE(this->server, "/rconSend").methods("POST"_method) ([&](const crow::request& req) {
        return this->handleRCON(req);
    });

    CROW_ROUTE(this->server, "/readFile").methods("POST"_method) ([&](const crow::request& req) {
        return this->handeReadFile(req);
    });

}

void WebServer::start(int port) {
    this->server.loglevel(crow::LogLevel::Debug);
    this->server.port(port).run();
}

void WebServer::createCommand(const char* name, std::function<std::string(const crow::request& req)> fn) {
    this->commands[name] = fn;
}

std::string WebServer::handleRCON(const crow::request& req) {

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
std::string WebServer::handeReadFile(const crow::request& req) {

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

    if (!std::filesystem::exists(filePath)) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 3;
        returnValue["message"] = "File doesnt exist.";
        return returnValue.dump();
    }
   

    std::ifstream file(filePath);
    if (!file.is_open()) {
        nlohmann::json returnValue;
        returnValue["status"] = "failed";
        returnValue["errorCode"] = 4;
        returnValue["message"] = "Failed to open file.";
        return returnValue.dump();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}