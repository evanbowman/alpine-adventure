#include "game.hpp"
#include "threadGuard.hpp"


int main() {
    auto context = Game::initialize();
    ThreadGuard logicThreadGuard([]() {
        Game::runUpdateLoop();
    });
    while (Game::isRunning()) {
        Game::display();
    }
    return 0;
}
