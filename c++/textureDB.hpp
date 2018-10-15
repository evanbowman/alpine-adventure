#pragma once

#include <SFML/Graphics.hpp>
#include <future>
#include <vector>

struct VideoContext;

class TextureDB {
public:
    sf::Texture * load(const std::string & name, VideoContext & vidCtx);
    sf::Texture * find(const std::string & name);

private:
    std::mutex texturesMutex_;
    std::map<std::string, sf::Texture> textures_;
};
