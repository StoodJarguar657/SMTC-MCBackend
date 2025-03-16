#include <Webserver.hpp>

int main() {

    WebServer server;
    server.start(25576);


    //// Have some custom commands
    //// Cleanup is neede later
    ////std::unordered_map<std::string, std::function<std::string>()
    //
    //
    //RCON rcon = RCON("192.168.2.133", 25575);
    //
    //// Only do 1 message per second
    //// If the password is wrong, 10 second timeout
    //
    //time_t nextMessagePossible = time(nullptr);
    //
    //crow::SimpleApp server;
    //CROW_ROUTE(server, "/rconSend").methods("POST"_method) ([&](const crow::request& req) {
    //
    //    time_t curTime = time(nullptr);
    //    if ((curTime - nextMessagePossible) < 0) {
    //        nlohmann::json returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 1;
    //        returnValue["message"] = "Wait a few seconds before the next message.";
    //        return returnValue.dump();
    //    }
    //
    //    nextMessagePossible = curTime + 1; // Set to 1 second
    //
    //    // "Authorization" should have the RCON password
    //    const auto& authorization = req.headers.find("Authorization");
    //    if (authorization == req.headers.end()) {
    //        nlohmann::json returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 2;
    //        returnValue["message"] = "Failed to find 'Authorization' in the header.";
    //        return returnValue.dump();
    //    }
    //
    //    const nlohmann::json& body = nlohmann::json::parse(req.body, nullptr, false, true);
    //    
    //    const auto& jsonMessage = body.find("message");
    //    if (jsonMessage == body.end()) {
    //        nlohmann::json returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 3;
    //        returnValue["message"] = "Failed to find 'message' in the body.";
    //        return returnValue.dump();
    //    }
    //
    //    // Check types of payload
    //    if (jsonMessage.value().type() != nlohmann::json::value_t::string) {
    //        nlohmann::json returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 4;
    //        returnValue["message"] = "Message is not a string.";
    //        return returnValue.dump();
    //    }
    //
    //    const std::string& rconPassword = authorization->second;
    //    const std::string& payload = jsonMessage->get<std::string>();
    //
    //    if (!rcon.authenticate(rconPassword)) {
    //        crow::json::wvalue returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 5;
    //        returnValue["message"] = "RCON password is invalid.";
    //        nextMessagePossible = curTime + 10; // Block for 10 seconds
    //        return returnValue.dump();
    //    }
    //
    //    std::string commandResponse;
    //    if (!rcon.sendConsoleCommand(payload, &commandResponse)) {
    //        crow::json::wvalue returnValue;
    //        returnValue["status"] = "failed";
    //        returnValue["errorCode"] = 6;
    //        returnValue["message"] = "Command is invalid.";
    //        return returnValue.dump();
    //    }
    //
    //    crow::json::wvalue returnValue;
    //    returnValue["status"] = "success";
    //    returnValue["message"] = commandResponse;
    //    return returnValue.dump();
    //});
    //
    //server.loglevel(crow::LogLevel::Debug);
    //server.port(25576).run();

	return 0;
}