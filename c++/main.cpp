#include "game.hpp"
#include "threadGuard.hpp"

int main() {
    auto context = Game::initialize();
    ThreadGuard logicThreadGuard([]() { Game::runUpdateLoop(); });
    Game::runVideoLoop();
    return 0;
}
