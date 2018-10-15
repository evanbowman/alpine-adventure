#include "camera.hpp"
#include "math.hpp"
#include "object.hpp"

namespace Game {

Camera::Camera(const sf::Vector2f & viewPort, const sf::Vector2u & windowSize)
    : overworldView_(sf::Vector2f(viewPort.x / 2, viewPort.y / 2), viewPort),
      windowSize_(windowSize), currentPosition_(overworldView_.getCenter()) {
    windowView_.setSize(windowSize_.x, windowSize_.y);
}

const sf::View & Camera::getWindowView() const { return windowView_; }

const sf::View & Camera::getOverworldView() const { return overworldView_; }

void Camera::setTarget(std::shared_ptr<Object> target) { target_ = target; }

void Camera::update(Microseconds delta) {
    if (not target_) {
        return;
    }
    overworldView_.setCenter(target_->getPosition());
    const float lerpSpeed = clamp(delta * 0.0000025f, 0.f, 1.f);
    currentPosition_ =
        lerp(target_->getPosition(), currentPosition_, lerpSpeed);
    const auto cameraTargetOffset = currentPosition_ - target_->getPosition();
    const sf::Vector2f windowViewSize = windowView_.getSize();
    const sf::Vector2f overworldViewSize = overworldView_.getSize();
    const sf::Vector2f scaleVec(windowViewSize.x / overworldViewSize.x,
                                windowViewSize.y / overworldViewSize.y);
    const sf::Vector2f scaledCameraOffset(cameraTargetOffset.x * scaleVec.x,
                                          cameraTargetOffset.y * scaleVec.y);
    windowView_.setCenter(windowSize_.x / 2.f + scaledCameraOffset.x,
                          windowSize_.y / 2.f + scaledCameraOffset.y);
}
}
