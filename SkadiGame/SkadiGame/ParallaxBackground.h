#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class ParallaxBackground {
public:
    void addLayer(const sf::Texture& tex, float factor, float y = 0.f) {
        Layer l;
        l.sprite.setTexture(tex);
        l.factor = factor;
        l.y = y;
        layers.push_back(l);
    }

    void update(sf::Vector2f cameraCenter, sf::Vector2f viewSize) {
        for (auto& l : layers) {
            float offset = -cameraCenter.x * l.factor;
            l.offsetX = offset;
            l.viewWidth = viewSize.x;
            l.camLeft = cameraCenter.x - viewSize.x / 2.f;
        }
    }

    void render(sf::RenderWindow& w) {
        for (auto& l : layers) {
            sf::Vector2u ts = l.sprite.getTexture()->getSize();
            if (ts.x == 0) continue;

            float x = std::fmod(l.offsetX, (float)ts.x);
            if (x > 0) x -= ts.x;

            for (float px = x; px < l.viewWidth; px += ts.x) {
                l.sprite.setPosition(px, l.y);
                w.draw(l.sprite);
            }
        }
    }

private:
    struct Layer {
        sf::Sprite sprite;
        float factor = 0.3f;
        float y = 0.f;
        float offsetX = 0.f;
        float viewWidth = 1280.f;
        float camLeft = 0.f;
    };
    std::vector<Layer> layers;
};
