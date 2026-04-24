#include "ParryWindow.h"
#include <algorithm>

ParryWindow::ParryWindow(float parryDuration, float cooldown)
    : PARRY_DURATION(parryDuration), COOLDOWN(cooldown) {
}

void ParryWindow::startBlock() {
    if (blocking || cooldownTimer > 0.f) return;
    blocking = true;
    blockTime = 0.f;
}

void ParryWindow::stopBlock() {
    if (!blocking) return;
    blocking = false;
    cooldownTimer = COOLDOWN;
}

void ParryWindow::update(float dt) {
    if (blocking)         blockTime += dt;
    if (cooldownTimer > 0.f) cooldownTimer -= dt;
    if (flashTimer > 0.f) flashTimer -= dt;
}

bool ParryWindow::isInParryWindow() const {
    return blocking && blockTime <= PARRY_DURATION;
}

ParryWindow::Result ParryWindow::judge() const {
    if (!blocking)                   return Result::NONE;
    if (blockTime <= PARRY_DURATION) return Result::PARRY;
    return Result::BLOCK;
}

float ParryWindow::getFlashAlpha() const {
    if (flashTimer <= 0.f) return 0.f;
    return flashTimer / FLASH_DURATION;
}

void ParryWindow::render(sf::RenderWindow& w, sf::Vector2f pos) {
    if (!blocking && cooldownTimer <= 0.f) return;

    sf::CircleShape dot(5.f);
    dot.setPosition(pos);

    if (isInParryWindow())
        dot.setFillColor(sf::Color::White);
    else if (blocking)
        dot.setFillColor(sf::Color(100, 150, 255));
    else
        dot.setFillColor(sf::Color(80, 80, 80));

    w.draw(dot);
}