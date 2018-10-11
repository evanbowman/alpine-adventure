#include "textureDB.hpp"


sf::Texture* TextureDB::load(const std::string& name, VideoContext& vidCtx) {
    std::lock_guard<std::mutex> guard(texturesMutex_);
    auto inserted = textures_.insert({name, sf::Texture{}});
    if (inserted.second) {
        if (not inserted.first->second.loadFromFile(name)) {
            // ...
        }
        return &inserted.first->second;
    } else {
        // ...
    }
    return nullptr;
}


sf::Texture* TextureDB::find(const std::string& name) {
    std::lock_guard<std::mutex> guard(texturesMutex_);
    auto found = textures_.find(name);
    if (found not_eq textures_.end()) {
        return &found->second;
    }
    return nullptr;
}
