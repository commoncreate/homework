#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>

class Camera {
public:
    Camera(sf::Vector2f viewSize) : view(sf::FloatRect(0, 0, viewSize.x, viewSize.y)) {}

    void setHorizontalMode(float fixedY) { horizontalMode = true; fixedCenterY = fixedY; }
    void setFreeMode() { horizontalMode = false; }

    void follow(sf::Vector2f target, float dt, float smoothness = 5.f) {
        sf::Vector2f cur = view.getCenter();
        float ty = horizontalMode ? fixedCenterY : target.y;
        sf::Vector2f diff = sf::Vector2f(target.x, ty) - cur;
        view.move(diff * std::min(1.f, smoothness * dt));
    }

    void clampToBounds(float mapW, float mapH) {
        sf::Vector2f half = view.getSize() / 2.f;
        sf::Vector2f c = view.getCenter();
        c.x = std::max(half.x, std::min(mapW - half.x, c.x));
        c.y = std::max(half.y, std::min(mapH - half.y, c.y));
        view.setCenter(c);
    }

    void shake(float intensity, float duration) {
        shakeTime = duration;
        shakeIntensity = intensity;
    }
    void updateShake(float dt) {
        if (shakeTime > 0.f) {
            shakeTime -= dt;
            float ox = (rand() % 100 - 50) / 50.f * shakeIntensity;
            float oy = (rand() % 100 - 50) / 50.f * shakeIntensity;
            view.move(ox, oy);
        }
    }

    const sf::View& get() const { return view; }
    sf::Vector2f getCenter() const { return view.getCenter(); }
    sf::Vector2f getSize() const { return view.getSize(); }

private:
    sf::View view;
    bool horizontalMode = false;
    float fixedCenterY = 360.f;
    float shakeTime = 0.f;
    float shakeIntensity = 0.f;
};