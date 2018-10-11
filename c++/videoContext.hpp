#pragma once

#include <vector>
#include <future>
#include <SFML/Graphics.hpp>


struct VideoContext {
    sf::VideoMode videoMode_;
    sf::RenderWindow window_;
    sf::RenderTexture shadowMap_;
    sf::RenderTexture world_;
    sf::Shader lightingShader;
    sf::Texture vignetteTexture_;
    sf::Sprite vignette_;

    VideoContext() : videoMode_(sf::VideoMode::getDesktopMode()),
                     window_(videoMode_, "Test", sf::Style::Fullscreen) {
        shadowMap_.create(videoMode_.width, videoMode_.height);
        world_.create(videoMode_.width, videoMode_.height);
        window_.setMouseCursorVisible(false);
        if (not lightingShader.loadFromFile("./glsl/lighting.vert",
                                            "./glsl/lighting.frag")) {
            throw std::runtime_error("failed to load shader");
        }
        vignetteTexture_.loadFromFile("./texture/vignette.png");
        vignette_.setTexture(vignetteTexture_);
        const auto windowSize = window_.getSize();
        vignette_.setScale(windowSize.x / 450.f, windowSize.y / 450.f);
        vignette_.setOrigin(225, 225);
        vignette_.setColor({255, 255, 255, 181});

    }
};
