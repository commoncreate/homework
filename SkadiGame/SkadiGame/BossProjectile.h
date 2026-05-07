#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "HitBox.h"

class BossProjectile {
public:
    BossProjectile(sf::Vector2f startPos, bool movingRight) {
        position = startPos;
        velocity.x = movingRight ? SPEED : -SPEED;
        this->movingRight = movingRight;
        life = MAX_LIFE;

        hitbox.damage = 20;
        hitbox.postureDamage = 18.f;
        hitbox.active = true;
        hitbox.lifeTime = MAX_LIFE;
        hitbox.ownerId = 0;   
    }

    void update(float dt) {
        position += velocity * dt;
        life -= dt;

        hitbox.rect = sf::FloatRect(
            position.x - 40.f, position.y - 25.f, 80.f, 50.f
        );
        if (life <= 0.f) hitbox.active = false;
    }

    void render(sf::RenderWindow& w) {
        if (!isAlive()) return;

        float dir = movingRight ? 1.f : -1.f;

        sf::ConvexShape moon;
        moon.setPointCount(8);
        float r = 38.f;
        moon.setPoint(0, { 0.f, -r });
        moon.setPoint(1, { r * 0.7f * dir, -r * 0.7f });
        moon.setPoint(2, { r * 1.0f * dir, 0.f });
        moon.setPoint(3, { r * 0.7f * dir, r * 0.7f });
        moon.setPoint(4, { 0.f, r });
        moon.setPoint(5, { r * 0.3f * dir, r * 0.5f });
        moon.setPoint(6, { r * 0.5f * dir, 0.f });
        moon.setPoint(7, { r * 0.3f * dir, -r * 0.5f });
        moon.setFillColor(sf::Color(255, 60, 40, 230));
        moon.setOutlineColor(sf::Color(255, 200, 100, 255));
        moon.setOutlineThickness(3.f);
        moon.setPosition(position);
        w.draw(moon);

        sf::ConvexShape inner;
        inner.setPointCount(8);
        float ir = 24.f;
        inner.setPoint(0, { 0.f, -ir });
        inner.setPoint(1, { ir * 0.7f * dir, -ir * 0.7f });
        inner.setPoint(2, { ir * 1.0f * dir, 0.f });
        inner.setPoint(3, { ir * 0.7f * dir, ir * 0.7f });
        inner.setPoint(4, { 0.f, ir });
        inner.setPoint(5, { ir * 0.3f * dir, ir * 0.5f });
        inner.setPoint(6, { ir * 0.5f * dir, 0.f });
        inner.setPoint(7, { ir * 0.3f * dir, -ir * 0.5f });
        inner.setFillColor(sf::Color(255, 200, 150, 220));
        inner.setPosition(position);
        w.draw(inner);

        for (int i = 0; i < 6; ++i) {
            sf::CircleShape particle(4.f + i * 0.5f);
            particle.setPosition(
                position.x - dir * (18.f + i * 14.f) - 4.f,
                position.y + (std::rand() % 24 - 12) - 4.f
            );
            sf::Uint8 a = (sf::Uint8)(220 - i * 30);
            particle.setFillColor(sf::Color(255, 80, 50, a));
            w.draw(particle);
        }
    }

    bool isAlive() const { return life > 0.f; }
    HitBox& getHitBox() { return hitbox; }
    void kill() { life = 0.f; hitbox.active = false; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool movingRight;
    float life;
    HitBox hitbox;

    static constexpr float SPEED = 550.f;
    static constexpr float MAX_LIFE = 1.2f;
};
