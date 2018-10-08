#include <iostream>

#include <SFML/Graphics.hpp>

#include "game.hpp"

// const auto desktop = sf::VideoMode::getDesktopMode();
// sf::RenderWindow window(desktop, "Test", sf::Style::Fullscreen);
// window.setMouseCursorVisible(false);
// sf::RenderTexture shadowMap;
// sf::RenderTexture world;
// shadowMap.create(desktop.width, desktop.height);
// world.create(desktop.width, desktop.height);
// sf::Texture faceTxtr, shadowTxtr;
// if (not faceTxtr.loadFromFile("Sprite-0001.png")) {
//     exit(1);
// }
// if (not shadowTxtr.loadFromFile("Sprite-0002.png")) {
//     exit(1);
// }
// auto guy = std::make_shared<Object>(faceTxtr, shadowTxtr);
// guy->setPosition({150, 150});

// Camera camera(guy, {desktop.width / 3.f, desktop.height / 3.f},
//               {desktop.width, desktop.height});

// world.display();
// window.setVerticalSyncEnabled(true);
// window.setFramerateLimit(60);
// if (!sf::Shader::isAvailable()) {
//     std::cerr << "shaders unsupported" << std::endl;
//     return 1;
// }

// sf::Shader lightingShader;
// if (!lightingShader.loadFromFile("lighting.vert", "lighting.frag")) {
//     std::cerr << "failed to load shader" << std::endl;
//     return 1;
// }

// sf::Clock clock;
// while (window.isOpen()) {
//     sf::Event event;
//     while (window.pollEvent(event)) {
//         if (event.type == sf::Event::Closed) {
//             window.close();
//         }
//     }
//     auto elapsed = clock.restart();

//     auto pos = guy->getPosition();
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
//         guy->setPosition({pos.x, pos.y - 2});
//     }
//     pos = guy->getPosition();
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
//         guy->setPosition({pos.x, pos.y + 2});
//     }
//     pos = guy->getPosition();
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
//         guy->setPosition({pos.x - 2, pos.y});
//     }
//     pos = guy->getPosition();
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
//         guy->setPosition({pos.x + 2, pos.y});
//     }
//     camera.update(elapsed);

//     shadowMap.clear();
//     shadowMap.setView(camera.getOverworldView());
//     guy->mapShadow(shadowMap);
//     shadowMap.display();

//     // world.clear({129, 178, 83}); // test green
//     world.clear({220, 220, 220}); // test white
//     // world.clear({227, 66, 52}); // test vermillion
//     world.setView(camera.getOverworldView());
//     guy->drawFace(world);
//     world.display();

//     window.clear();
//     window.setView(camera.getWindowView());

//     sf::Sprite shadowMapSprite(shadowMap.getTexture());
//     lightingShader.setUniform("shadowMap", sf::Shader::CurrentTexture);
//     lightingShader.setUniform("world", world.getTexture());
//     window.draw(shadowMapSprite, &lightingShader);

//     window.display();
// }

// return 0;


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
