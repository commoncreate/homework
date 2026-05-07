#pragma once
#include <SFML/Graphics.hpp>

class ParryWindow {
public:
    enum class Result { NONE, PARRY, BLOCK };

    ParryWindow(float parryDuration = 0.15f, float cooldown = 0.4f);

    void startBlock();
    void stopBlock();
    void update(float dt);

    Result judge() const;

    bool  isBlocking() const { return blocking; }
    bool  isInParryWindow() const;
    bool  isOnCooldown() const { return cooldownTimer > 0.f; }
    float getBlockTime() const { return blockTime; }

    void  triggerParryFlash() { flashTimer = FLASH_DURATION; }
    float getFlashAlpha() const;

    void  render(sf::RenderWindow& w, sf::Vector2f pos);

private:
    const float PARRY_DURATION;
    const float COOLDOWN;
    const float FLASH_DURATION = 0.2f;

    bool  blocking = false;
    float blockTime = 0.f;
    float cooldownTimer = 0.f;
    float flashTimer = 0.f;
};