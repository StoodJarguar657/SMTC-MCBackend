#include "pch.h"
#include <MCBackend.hpp>

int main() {
    
    MCBackend backend;
    
    MCServerDesc testServer;
    testServer.startLogic = SERVER_DESC_START::PROXY_SERVER;
    testServer.stopLogic = SERVER_DESC_STOP::AUTO_STOP;
    testServer.autoStopAfter = 20;
    testServer.serverFolder = "C:\\Users\\emanu\\Desktop\\Test server\\Server1\\";
    if (!backend.addServer(&testServer))
        return 1;

    if (!backend.initialize(25576))
        return 1;

    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}