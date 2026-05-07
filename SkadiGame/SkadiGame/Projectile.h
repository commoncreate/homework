#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "HitBox.h"

class Projectile {
public:
    Projectile(sf::Vector2f startPos, bool facingRight) {
        position = startPos;
        velocity.x = facingRight ? SPEED : -SPEED;
        this->facingRight = facingRight;
        life = MAX_LIFE;   

        hitbox.damage = 15;
        hitbox.postureDamage = 15.f;
        hitbox.active = true;
        hitbox.lifeTime = MAX_LIFE;
        hitbox.ownerId = 1;
        hitbox.isSkill = true;
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

        float t = 1.f - life / MAX_LIFE;     
        float dir = facingRight ? 1.f : -1.f;

        sf::ConvexShape moon;
        moon.setPointCount(8);
        float r = 35.f;
        moon.setPoint(0, { 0.f, -r });
        moon.setPoint(1, { r * 0.7f * dir, -r * 0.7f });
        moon.setPoint(2, { r * 1.0f * dir, 0.f });
        moon.setPoint(3, { r * 0.7f * dir, r * 0.7f });
        moon.setPoint(4, { 0.f, r });
        moon.setPoint(5, { r * 0.3f * dir, r * 0.5f });
        moon.setPoint(6, { r * 0.5f * dir, 0.f });
        moon.setPoint(7, { r * 0.3f * dir, -r * 0.5f });
        moon.setFillColor(sf::Color(100, 200, 255, 220));
        moon.setOutlineColor(sf::Color(255, 255, 255, 255));
        moon.setOutlineThickness(2.f);
        moon.setPosition(position);
        moon.setRotation(rotation);
        w.draw(moon);

        sf::ConvexShape inner;
        inner.setPointCount(8);
        float ir = 22.f;
        inner.setPoint(0, { 0.f, -ir });
        inner.setPoint(1, { ir * 0.7f * dir, -ir * 0.7f });
        inner.setPoint(2, { ir * 1.0f * dir, 0.f });
        inner.setPoint(3, { ir * 0.7f * dir, ir * 0.7f });
        inner.setPoint(4, { 0.f, ir });
        inner.setPoint(5, { ir * 0.3f * dir, ir * 0.5f });
        inner.setPoint(6, { ir * 0.5f * dir, 0.f });
        inner.setPoint(7, { ir * 0.3f * dir, -ir * 0.5f });
        inner.setFillColor(sf::Color(220, 240, 255, 200));
        inner.setPosition(position);
        inner.setRotation(rotation);
        w.draw(inner);

        for (int i = 0; i < 5; ++i) {
            sf::CircleShape particle(3.f + i * 0.5f);
            particle.setPosition(
                position.x - dir * (15.f + i * 12.f) - 3.f,
                position.y + (std::rand() % 20 - 10) - 3.f
            );
            sf::Uint8 a = (sf::Uint8)(200 - i * 35);
            particle.setFillColor(sf::Color(150, 220, 255, a));
            w.draw(particle);
        }
    }

    bool isAlive() const { return life > 0.f; }
    HitBox& getHitBox() { return hitbox; }
    sf::Vector2f getPosition() const { return position; }

    void kill() { life = 0.f; hitbox.active = false; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool facingRight;
    float rotation = 0.f;
    float life;
    HitBox hitbox;

    static constexpr float SPEED = 600.f;     
    static constexpr float MAX_LIFE = 0.8f;    
public:
    float getLife() const { return life; }
    void setLife(float l) { life = l; }
private:
};
