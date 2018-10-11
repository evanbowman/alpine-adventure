#pragma once

#include <vector>
#include <future>
#include <SFML/Graphics.hpp>

struct VideoContext;


class TextureDB {
public:
    sf::Texture* load(const std::string& name, VideoContext& vidCtx);
    sf::Texture* find(const std::string& name);

private:
    std::mutex texturesMutex_;
    std::map<std::string, sf::Texture> textures_;
};
