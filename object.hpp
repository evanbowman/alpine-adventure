#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

// FIXME: all this stuff needs to be better organized


class GraphicsComponent {
public:
    GraphicsComponent(const sf::Texture& faceTexture,
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

private:
    sf::Sprite faceSprite_;
    sf::Sprite shadowSprite_;
};


class Object {
public:
    Object() {}

    void setPosition(const sf::Vector2f& pos) {
        position_ = pos;
    }

    sf::Vector2f getPosition() {
        return position_;
    }

    void setGraphicsComponent(std::unique_ptr<GraphicsComponent> gfx) {
        gfx_ = std::move(gfx);
    }

    GraphicsComponent* getGraphicsComponent() {
        return gfx_.get();
    }

private:
    sf::Vector2f position_;
    std::unique_ptr<GraphicsComponent> gfx_;
};

using ObjectPtr = std::shared_ptr<Object>;
