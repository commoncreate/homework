#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

class AssetManager {
public:
    static AssetManager& instance() {
        static AssetManager inst;
        return inst;
    }

    const sf::Texture& loadTexture(const std::string& name, const std::string& path) {
        auto it = textures.find(name);
        if (it != textures.end()) return it->second;

        sf::Texture tex;
        if (!tex.loadFromFile(path)) {
            std::cerr << "[ERROR] 无法加载贴图: " << path << std::endl;
        }
        textures[name] = std::move(tex);
        return textures[name];
    }

    const sf::Texture& get(const std::string& name) {
        return textures.at(name);
    }

private:
    AssetManager() = default;
    std::unordered_map<std::string, sf::Texture> textures;
};