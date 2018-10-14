#pragma once
#include <SFML/Graphics.hpp>


namespace Game {

    class Sprite {
    public:
        Sprite(const sf::Texture& texture) : sprite_(texture) {}

        void display(sf::RenderTarget& target) {
            target.draw(sprite_);
        }

        void setPosition(const sf::Vector2f& pos) {
            sprite_.setPosition(pos);
        }

        void setScale(const sf::Vector2f& scale) {
            sprite_.setScale(scale);
        }

        void setColor(const sf::Color& color) {
            sprite_.setColor(color);
        }

    private:
        sf::Sprite sprite_;
    };


}
