#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class EnemyPlaceholder {
public:
    EnemyPlaceholder(sf::Vector2f pos) : position(pos) {
        shape.setSize({ 40.f, 60.f });
        shape.setFillColor(sf::Color(180, 60, 60));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2.f);
        shape.setOrigin(20.f, 60.f);
    }

    void update(float dt) {
        breatheTimer += dt;
        float sy = 1.f + std::sin(breatheTimer * 2.f) * 0.05f;
        shape.setScale(1.f, sy);
    }

    void render(sf::RenderWindow& w) {
        shape.setPosition(position);
        w.draw(shape);
    }

    sf::Vector2f getPos() const { return position; }

private:
    sf::Vector2f position;
    sf::RectangleShape shape;
    float breatheTimer = 0.f;
};