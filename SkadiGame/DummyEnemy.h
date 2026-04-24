#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"

class DummyEnemy {
public:
    DummyEnemy(sf::Vector2f pos) : position(pos) {
        shape.setSize({ 40.f, 60.f });
        shape.setFillColor(sf::Color(200, 100, 100));
        shape.setOrigin(20.f, 60.f);
    }

    void update(float dt, sf::Vector2f playerPos) {
        attackTimer -= dt;
        if (attackTimer <= 0.f) {
            float dir = (playerPos.x < position.x) ? -1.f : 1.f;
            float atkX = position.x + dir * 30.f - 30.f;
            float atkY = position.y - 45.f;
            hitbox.rect = sf::FloatRect(atkX, atkY, 60.f, 40.f);
            hitbox.damage = 15;
            hitbox.postureDamage = 12.f;
            hitbox.active = true;
            hitbox.lifeTime = 0.25f;
            attackTimer = 2.f;
        }
        hitbox.update(dt);
    }

    void render(sf::RenderWindow& w) {
        shape.setPosition(position);
        w.draw(shape);
        if (hitbox.active) {
            sf::RectangleShape vis({ hitbox.rect.width, hitbox.rect.height });
            vis.setPosition(hitbox.rect.left, hitbox.rect.top);
            vis.setFillColor(sf::Color(255, 0, 0, 120));
            w.draw(vis);
        }
    }

    HitBox& getHitBox() { return hitbox; }
    void setPosition(sf::Vector2f p) { position = p; }

private:
    sf::Vector2f position;
    sf::RectangleShape shape;
    HitBox hitbox;
    float attackTimer = 2.f;
};