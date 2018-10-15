#pragma once
#include <SFML/Graphics.hpp>

namespace Game {

class Sprite {
public:
    using Keyframe = size_t;

    Sprite(const sf::Texture & texture) : sprite_(texture), keyframe_(0) {}

    const sf::Sprite & alias() const { return sprite_; }

    void setPosition(const sf::Vector2f & pos) { sprite_.setPosition(pos); }

    void setScale(const sf::Vector2f & scale) { sprite_.setScale(scale); }

    void setColor(const sf::Color & color) { sprite_.setColor(color); }

    void setSubRect(const sf::IntRect & rect) {
        sprite_.setTextureRect(rect);
        subRect_ = rect;
    }

    void setKeyframe(Keyframe keyframe) {
        auto rect = sprite_.getTextureRect();
        rect.left += rect.width * (keyframe - keyframe_);
        keyframe_ = keyframe;
        sprite_.setTextureRect(rect);
    }

private:
    sf::Sprite sprite_;
    sf::IntRect subRect_;
    size_t keyframe_;
};
}
