#pragma once

#include <SFML/Graphics.hpp>
#include <future>
#include <vector>

struct VideoContext {
    sf::VideoMode videoMode_;
    sf::RenderWindow window_;
    sf::RenderTexture shadowMap_;
    sf::RenderTexture world_;
    sf::Shader lightingShader;

    VideoContext()
        : videoMode_(sf::VideoMode::getDesktopMode()),
          window_(videoMode_, "Test", sf::Style::Fullscreen) {
        shadowMap_.create(videoMode_.width, videoMode_.height);
        world_.create(videoMode_.width, videoMode_.height);
        window_.setMouseCursorVisible(false);
        if (not lightingShader.loadFromFile("./glsl/lighting.vert",
                                            "./glsl/lighting.frag")) {
            throw std::runtime_error("failed to load shader");
        }
    }
};
