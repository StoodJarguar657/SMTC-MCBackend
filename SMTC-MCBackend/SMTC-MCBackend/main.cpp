#include <MCBackend.hpp>

int main() {
    
    MCBackend backend;
    
    MCServerDesc testServer;
    testServer.autoStopAfter = 20;
    
    testServer.serverFolder = "/home/stood/sambashare/SMTC main - Test/";
    if (!backend.addServer(testServer))
        return 1;

    if (!backend.initialize(25576))
        return 1;

    while (true) {
        backend.update();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}
