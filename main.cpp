#include <iostream>

#include <SFML/Graphics.hpp>

#include "game.hpp"


int main() {
    Game game;
    sf::Clock deltaTimer;
    while (game.isRunning()) {
        const sf::Time elapsed = deltaTimer.restart();
        game.update(elapsed.asMicroseconds());
        game.display();
    }
    return 0;
}
