#pragma once

#include <SFML/Graphics.hpp>
#include <memory>


class Object {
public:
    Object(const sf::Texture& faceTexture,
           const sf::Texture& shadowTexture) :
        faceSprite_(faceTexture),
        shadowSprite_(shadowTexture) {}

    void drawFace(sf::RenderTarget& target) {
        target.draw(faceSprite_);
    }

    void mapShadow(sf::RenderTarget& target) {
        target.draw(shadowSprite_);
    }

    void setPosition(const sf::Vector2f& pos) {
        faceSprite_.setPosition(pos);
        shadowSprite_.setPosition(pos);
    }

    sf::Vector2f getPosition() {
        return faceSprite_.getPosition();
    }

private:
    sf::Sprite faceSprite_;
    sf::Sprite shadowSprite_;
};

using ObjectPtr = std::shared_ptr<Object>;
