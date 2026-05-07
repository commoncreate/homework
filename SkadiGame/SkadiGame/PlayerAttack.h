#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"

class PlayerAttack {
public:
    enum class Phase { IDLE, STARTUP, ACTIVE, RECOVERY };

    void startAttack(sf::Vector2f playerPos, bool facingRight) {
        if (phase != Phase::IDLE) return;
        phase = Phase::STARTUP;
        timer = STARTUP_TIME;
        this->facingRight = facingRight;
        this->playerPos = playerPos;
    }

    void update(float dt, sf::Vector2f playerPos) {
        this->playerPos = playerPos;
        if (phase == Phase::IDLE) return;
        timer -= dt;

        if (phase == Phase::STARTUP && timer <= 0.f) {
            phase = Phase::ACTIVE;
            timer = ACTIVE_TIME;
            spawnHitbox();
        }
        else if (phase == Phase::ACTIVE && timer <= 0.f) {
            phase = Phase::RECOVERY;
            timer = RECOVERY_TIME;
            hitbox.active = false;
        }
        else if (phase == Phase::RECOVERY && timer <= 0.f) {
            phase = Phase::IDLE;
        }
        hitbox.update(dt);
    }

    void render(sf::RenderWindow& w) {
        if (hitbox.active) {
            sf::RectangleShape r({ hitbox.rect.width, hitbox.rect.height });
            r.setPosition(hitbox.rect.left, hitbox.rect.top);
            r.setFillColor(sf::Color(100, 200, 255, 150));
            w.draw(r);
        }
    }

    HitBox& getHitBox() { return hitbox; }
    bool isAttacking() const { return phase != Phase::IDLE; }
    bool canCancel() const { return phase == Phase::RECOVERY; }

private:
    Phase phase = Phase::IDLE;
    float timer = 0.f;
    bool facingRight = true;
    sf::Vector2f playerPos;
    HitBox hitbox;

    const float STARTUP_TIME = 0.12f;
    const float ACTIVE_TIME = 0.15f;
    const float RECOVERY_TIME = 0.25f;

    void spawnHitbox() {
        float dir = facingRight ? 1.f : -1.f;
        hitbox.rect = sf::FloatRect(
            playerPos.x + dir * 10.f - (facingRight ? 0.f : 80.f),
            playerPos.y - 60.f,
            80.f, 60.f
        );
        hitbox.damage = 15;
        hitbox.postureDamage = 20.f;
        hitbox.active = true;
        hitbox.lifeTime = ACTIVE_TIME;
        hitbox.ownerId = 1;
    }
};

