#include "Animation.h"

void Animation::setup(const sf::Texture& t, int fc, int fw, int fh, float fd, bool lp) {
    tex = &t; frameCount = fc; frameW = fw; frameH = fh;
    frameDuration = fd; loop = lp;
    reset();
}

void Animation::reset() { currentFrame = 0; timer = 0.f; finished = false; }

void Animation::update(float dt) {
    if (finished) return;
    timer += dt;
    if (timer >= frameDuration) {
        timer -= frameDuration;
        currentFrame++;
        if (currentFrame >= frameCount) {
            if (loop) currentFrame = 0;
            else { currentFrame = frameCount - 1; finished = true; }
        }
    }
}

void Animation::applyTo(sf::Sprite& sprite) const {
    if (!tex) return;
    sprite.setTexture(*tex);
    sprite.setTextureRect(sf::IntRect(currentFrame * frameW, 0, frameW, frameH));
}