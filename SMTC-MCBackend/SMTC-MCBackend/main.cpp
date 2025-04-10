#include <MCBackend.hpp>

int main() {
    
    MCBackend backend;
    
    MCServerDesc testServer;
    testServer.startLogic = SERVER_DESC_START::PROXY_SERVER;
    testServer.stopLogic = SERVER_DESC_STOP::AUTO_STOP;
    testServer.autoStopAfter = 20;
    testServer.serverFolder = "/home/stood/sambashare/SMTC test/";
    if (!backend.addServer(testServer))
        return 1;
    
    MCServerDesc mainServer;
    mainServer.startLogic = SERVER_DESC_START::KEEP_ONLINE;
    mainServer.stopLogic = SERVER_DESC_STOP::STOP_NONE;
    mainServer.serverFolder = "/home/stood/sambashare/SMTC main/";
    if (!backend.addServer(mainServer))
        return 1;

    if (!backend.initialize(25576))
        return 1;

    while (true) {
        backend.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

	return 0;
}
