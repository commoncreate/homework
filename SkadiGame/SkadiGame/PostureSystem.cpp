#include "PostureSystem.h"
#include <algorithm>

PostureSystem::PostureSystem(float m) : maxValue(m) {}

void PostureSystem::addPosture(float a) {
    if (broken) return;
    current = std::min(current + a, maxValue);
    timeSinceHit = 0.f;
    if (current >= maxValue) {
        broken = true;
        justBrokenFlag = true;
        brokenTimer = BROKEN_DURATION;
    }
}

void PostureSystem::update(float dt, float hpPercent) {
    timeSinceHit += dt;
    if (broken) {
        brokenTimer -= dt;
        if (brokenTimer <= 0.f) {
            broken = false;
            current = maxValue * 0.7f;
        }
        return;
    }
    float mul = 0.3f + 0.7f * hpPercent;
    if (timeSinceHit >= regenDelay) {
        current = std::max(0.f, current - regenRate * mul * dt);
    }
}

void PostureSystem::reset() {
    current = 0.f; broken = false; brokenTimer = 0.f;
}

void PostureSystem::render(sf::RenderWindow& w, sf::Vector2f pos, float width) {
    float h = 8.f;
    sf::RectangleShape bg({ width, h });
    bg.setPosition(pos);
    bg.setFillColor(sf::Color(30, 30, 30, 200));
    bg.setOutlineColor(sf::Color::Black);
    bg.setOutlineThickness(1.f);
    w.draw(bg);

    float p = getPercent();
    sf::RectangleShape fg({ width * p, h });
    fg.setPosition(pos);

    if (broken) {
        int flash = (int)(brokenTimer * 10) % 2;
        fg.setFillColor(flash ? sf::Color::White : sf::Color(255, 200, 100));
    }
    else if (p < 0.5f) fg.setFillColor(sf::Color(255, 220, 50));
    else if (p < 0.8f) fg.setFillColor(sf::Color(255, 140, 20));
    else fg.setFillColor(sf::Color(255, 60, 60));

    w.draw(fg);
}
