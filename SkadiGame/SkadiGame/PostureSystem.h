#pragma once
#include <SFML/Graphics.hpp>

class PostureSystem {
public:
    PostureSystem(float maxValue = 100.f);

    void addPosture(float amount);
    void update(float dt, float hpPercent);
    void reset();

    bool isBroken() const { return broken; }
    bool justBroken() {
        bool jb = justBrokenFlag;
        justBrokenFlag = false;
        return jb;
    }
    float getPercent() const { return current / maxValue; }
    float getCurrent() const { return current; }
    float getMax() const { return maxValue; }

    void render(sf::RenderWindow& w, sf::Vector2f pos, float width = 80.f);

private:
    float current = 0.f;
    float maxValue;
    float regenRate = 15.f;
    float regenDelay = 0.8f;
    float timeSinceHit = 0.f;
    bool broken = false;
    bool justBrokenFlag = false;
    float brokenTimer = 0.f;
    const float BROKEN_DURATION = 2.5f;
};

