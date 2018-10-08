#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "types.hpp"

class Object;


class Camera {
public:
    Camera(const sf::Vector2f& viewPort,
           const sf::Vector2u& windowSize);

    void update(Microseconds delta);

    void setTarget(std::shared_ptr<Object> target);

    const sf::View& getWindowView() const {
        return windowView_;
    }

    const sf::View& getOverworldView() const {
        return overworldView_;
    }

private:
    sf::View overworldView_;
    sf::View windowView_;
    sf::Vector2u windowSize_;
    std::shared_ptr<Object> target_;
    sf::Vector2f currentPosition_;
};
