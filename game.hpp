#pragma once

#include <list>
#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"


class Game {
public:
    Game();

    void update(Microseconds delta);

    void display();

    bool isRunning() const;

private:
    sf::RenderWindow window_;
    sf::RenderTexture shadowMap_;
    sf::RenderTexture world_;
    Camera camera_;
    std::list<ObjectPtr> gameObjects_;
    sf::Shader lightingShader;
    sf::Texture testFace_, testShadow_;
};
