#pragma once

#include <SFML/Graphics.hpp>
#include <memory>


class Sprite {
public:
    Sprite(const sf::Texture& texture) : sprite_(texture) {}

    void display(sf::RenderTarget& target) {
        target.draw(sprite_);
    }

    void setPosition(const sf::Vector2f& pos) {
        sprite_.setPosition(pos);
    }

private:
    sf::Sprite sprite_;
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

    void setFace(std::unique_ptr<Sprite> face) {
        face_ = std::move(face);
    }

    Sprite* getFace() {
        return face_.get();
    }

    void setShadow(std::unique_ptr<Sprite> shadow) {
        shadow_ = std::move(shadow);
    }

    Sprite* getShadow() {
        return shadow_.get();
    }

private:
    sf::Vector2f position_;
    std::unique_ptr<Sprite> face_;
    std::unique_ptr<Sprite> shadow_;
};

using ObjectPtr = std::shared_ptr<Object>;
