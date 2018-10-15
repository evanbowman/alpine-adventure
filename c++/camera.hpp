#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "types.hpp"

namespace Game {

class Object;

class Camera {
public:
    Camera(const sf::Vector2f & viewPort, const sf::Vector2u & windowSize);

    void update(Microseconds delta);

    void setTarget(std::shared_ptr<Object> target);

    const sf::View & getWindowView() const;

    const sf::View & getOverworldView() const;

private:
    sf::View overworldView_;
    sf::View windowView_;
    sf::Vector2u windowSize_;
    std::shared_ptr<Object> target_;
    sf::Vector2f currentPosition_;
};
}
