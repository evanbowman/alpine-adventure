#include "game.hpp"


static const auto desktopMode = sf::VideoMode::getDesktopMode();


Game::Game() : window_(desktopMode, "Test", sf::Style::Fullscreen),
               camera_({desktopMode.width / 3.f, desktopMode.height / 3.f},
                       {desktopMode.width, desktopMode.height}) {
    shadowMap_.create(desktopMode.width, desktopMode.height);
    world_.create(desktopMode.width, desktopMode.height);
    testFace_.loadFromFile("Sprite-0001.png");
    testShadow_.loadFromFile("Sprite-0002.png");
    auto obj = std::make_shared<Object>(testFace_, testShadow_);
    obj->setPosition({150, 150});
    camera_.setTarget(obj);
    gameObjects_.push_back(obj);
    window_.setVerticalSyncEnabled(true);
    window_.setFramerateLimit(60);
    if (not sf::Shader::isAvailable()) {
        throw std::runtime_error("unable to run without shaders");
    }
    lightingShader.loadFromFile("lighting.vert", "lighting.frag");
}


void Game::update(Microseconds delta) {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        }
    }
    auto& guy = gameObjects_.front();
    auto pos = guy->getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        guy->setPosition({pos.x, pos.y - 2});
    }
    pos = guy->getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        guy->setPosition({pos.x, pos.y + 2});
    }
    pos = guy->getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        guy->setPosition({pos.x - 2, pos.y});
    }
    pos = guy->getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        guy->setPosition({pos.x + 2, pos.y});
    }
    camera_.update(delta);
}


void Game::display() {
    shadowMap_.clear();
    shadowMap_.setView(camera_.getOverworldView());
    world_.clear({220, 220, 220});
    world_.setView(camera_.getOverworldView());
    for (auto& object : gameObjects_) {
        object->drawFace(world_);
        object->mapShadow(shadowMap_);
    }
    world_.display();
    shadowMap_.display();

    window_.clear();
    window_.setView(camera_.getWindowView());

    sf::Sprite shadowMapSprite(shadowMap_.getTexture());
    lightingShader.setUniform("shadowMap", sf::Shader::CurrentTexture);
    lightingShader.setUniform("world", world_.getTexture());
    window_.draw(shadowMapSprite, &lightingShader);
    window_.display();
}


bool Game::isRunning() const {
    return window_.isOpen();
}
