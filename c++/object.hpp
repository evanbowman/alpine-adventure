#pragma once

#include "collider.hpp"
#include "sprite.hpp"
#include <memory>

namespace Game {

class Object {
public:
    Object() : zOrder_(0), visible_(true) {}

    void setPosition(const sf::Vector2f & pos) { position_ = pos; }

    sf::Vector2f getPosition() { return position_; }

    void setFace(std::unique_ptr<Sprite> face) { face_ = std::move(face); }

    Sprite * getFace() { return face_.get(); }

    void setShadow(std::unique_ptr<Sprite> shadow) {
        shadow_ = std::move(shadow);
    }

    Sprite * getShadow() { return shadow_.get(); }

    void setCollider(std::unique_ptr<Collider> collider) {
        collider_ = std::move(collider);
    }

    Collider * getCollider() { return collider_.get(); }

    bool isVisible() const { return visible_; }

    void setVisible(bool visible) { visible_ = visible; }

    void setZOrder(int z) { zOrder_ = z; }

    int getZOrder() const { return zOrder_; }

private:
    sf::Vector2f position_;
    std::unique_ptr<Sprite> face_;
    std::unique_ptr<Sprite> shadow_;
    std::unique_ptr<Collider> collider_;
    int zOrder_;
    bool visible_;
};

using ObjectPtr = std::shared_ptr<Object>;
}
