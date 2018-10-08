#include "camera.hpp"
#include "object.hpp"

namespace math {
inline float clamp(float x, float floor, float ceil) {
    if (x < floor) {
        return floor;
    } else if (x > ceil) {
        return ceil;
    } else {
        return x;
    }
}
template <typename T> T lerp(const T & A, const T & B, const float t) {
    return A * t + (1 - t) * B;
}
inline float lerp(const float A, const float B, const float t) {
    return A * t + (1 - t) * B;
}
inline float smoothstep(const float edge0, const float edge1, float x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return x * x * (3 - 2 * x);
}
inline float smootherstep(const float edge0, const float edge1, float x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return x * x * x * (x * (x * 6 - 15) + 10);
}
}



Camera::Camera(const sf::Vector2f& viewPort,
               const sf::Vector2u& windowSize) :
    overworldView_(sf::Vector2f(viewPort.x / 2, viewPort.y / 2), viewPort),
    windowSize_(windowSize),
    currentPosition_(overworldView_.getCenter()) {
    windowView_.setSize(windowSize_.x, windowSize_.y);
    // windowView_.zoom(0.75);
}


void Camera::setTarget(std::shared_ptr<Object> target) {
    target_ = target;
}


void Camera::update(Microseconds delta) {
    overworldView_.setCenter(target_->getPosition());
    const float lerpSpeed = math::clamp(delta * 0.0000025f,
                            0.f, 1.f);
    currentPosition_ =
        math::lerp(target_->getPosition(), currentPosition_, lerpSpeed);

    // Note: I wrote this view transform code years ago, and never documented
    // anything. I have no idea how this code works, and because it works, no
    // interest in figuring it out.
    const sf::Vector2f cameraTargetOffset = currentPosition_ - target_->getPosition();
    const sf::Vector2f windowViewSize = windowView_.getSize();
    const sf::Vector2f overworldViewSize = overworldView_.getSize();
    const sf::Vector2f scaleVec(windowViewSize.x / overworldViewSize.x,
                                windowViewSize.y / overworldViewSize.y);
    const sf::Vector2f scaledCameraOffset(cameraTargetOffset.x * scaleVec.x,
                                          cameraTargetOffset.y * scaleVec.y);
    windowView_.setCenter(windowSize_.x / 2.f + scaledCameraOffset.x,
                          windowSize_.y / 2.f + scaledCameraOffset.y);
}
