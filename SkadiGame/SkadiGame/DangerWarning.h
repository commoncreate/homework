#pragma once
#include <SFML/Graphics.hpp>

class DangerWarning {
public:
    void trigger(float duration = 0.8f) {
        active = true;
        timer = duration;
        totalDuration = duration;
    }

    void update(float dt) {
        if (active) {
            timer -= dt;
            if (timer <= 0.f) active = false;
        }
    }

    void render(sf::RenderWindow& w, sf::Vector2f bossScreenPos) {
        if (!active) return;
        float phase = timer / totalDuration;
        int flash = (int)(timer * 15) % 2;
        sf::Color col = flash ? sf::Color(255, 50, 50) : sf::Color(255, 200, 50);

        sf::RectangleShape bg({ 60.f, 60.f });
        bg.setPosition(bossScreenPos.x - 30.f, bossScreenPos.y - 100.f);
        bg.setFillColor(sf::Color(0, 0, 0, 200));
        bg.setOutlineColor(col);
        bg.setOutlineThickness(4.f);
        w.draw(bg);

        sf::RectangleShape bar1({ 8.f, 30.f });
        bar1.setPosition(bossScreenPos.x - 4.f, bossScreenPos.y - 90.f);
        bar1.setFillColor(col);
        w.draw(bar1);

        sf::CircleShape dot(4.f);
        dot.setPosition(bossScreenPos.x - 4.f, bossScreenPos.y - 55.f);
        dot.setFillColor(col);
        w.draw(dot);
    }

    bool isActive() const { return active; }

private:
    bool active = false;
    float timer = 0.f;
    float totalDuration = 0.8f;
};

