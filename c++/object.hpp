#pragma once

#include <memory>
#include "sprite.hpp"


namespace Game {

    class Object {
    public:
        Object() : visible_(true) {}

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

        bool isVisible() const {
            return visible_;
        }

        void setVisible(bool visible) {
            visible_ = visible;
        }

    private:
        sf::Vector2f position_;
        std::unique_ptr<Sprite> face_;
        std::unique_ptr<Sprite> shadow_;
        bool visible_;
    };


    using ObjectPtr = std::shared_ptr<Object>;


}
