#include "Player.h"
#include "AssetManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Player::Player() { position = { 200.f, 400.f }; }

void Player::loadAssets() {
    auto& AM = AssetManager::instance();
    const int FW = 180, FH = 180;   // LuizMelo HeroKnight 每帧 180×180

    const sf::Texture& tIdle = AM.loadTexture("skadi_idle", "assets/sprites/skadi_idle.png");
    const sf::Texture& tRun = AM.loadTexture("skadi_run", "assets/sprites/skadi_run.png");
    const sf::Texture& tJump = AM.loadTexture("skadi_jump", "assets/sprites/skadi_jump.png");
    const sf::Texture& tFall = AM.loadTexture("skadi_fall", "assets/sprites/skadi_fall.png");
    const sf::Texture& tBlock = AM.loadTexture("skadi_block", "assets/sprites/skadi_block.png");
    const sf::Texture& tHurt = AM.loadTexture("skadi_hurt", "assets/sprites/skadi_hurt.png");

    animIdle.setup(tIdle, 10, FW, FH, 0.10f, true);
    animRun.setup(tRun, 10, FW, FH, 0.08f, true);
    animJump.setup(tJump, 3, FW, FH, 0.10f, false);
    animFall.setup(tFall, 3, FW, FH, 0.10f, false);
    animBlock.setup(tBlock, 5, FW, FH, 0.08f, false);
    animHurt.setup(tHurt, 3, FW, FH, 0.10f, false);

    sprite.setOrigin(FW / 2.f, 135.f);
    sprite.setScale(0.8f, 0.8f);
    applyCurrentAnim();
}

Animation& Player::currentAnim() {
    switch (state) {
    case PlayerState::RUN:           return animRun;
    case PlayerState::JUMP:          return animJump;
    case PlayerState::FALL:          return animFall;
    case PlayerState::BLOCK:
    case PlayerState::PARRY_SUCCESS: return animBlock;
    case PlayerState::HURT:          return animHurt;
    default:                         return animIdle;
    }
}

void Player::applyCurrentAnim() { currentAnim().applyTo(sprite); }

sf::FloatRect Player::getBounds() const {
    return { position.x - hitboxSize.x / 2.f, position.y - hitboxSize.y,
             hitboxSize.x, hitboxSize.y };
}

void Player::handleInput(float dt) {
    if (state == PlayerState::PARRY_SUCCESS) return;

    velocity.x = 0.f;
    float mul = parry.isBlocking() ? 0.4f : 1.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -MOVE_SPEED * mul; facingRight = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = MOVE_SPEED * mul; facingRight = true;
    }

    bool spNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spNow && !spacePrev && !parry.isBlocking()) {
        if (onGround) {
            velocity.y = JUMP_FORCE;
            onGround = false; canDoubleJump = true;
        }
        else if (canDoubleJump) {
            velocity.y = JUMP_FORCE * 0.85f;
            canDoubleJump = false;
        }
    }
    spacePrev = spNow;

    bool jNow = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
    if (jNow && !jPrev)      parry.startBlock();
    else if (!jNow && jPrev) parry.stopBlock();
    jPrev = jNow;
}

void Player::update(float dt) {
    parry.update(dt);

    if (state == PlayerState::PARRY_SUCCESS) {
        parrySuccessTimer -= dt;
        velocity.x = 0.f;
        if (parrySuccessTimer <= 0.f) state = PlayerState::IDLE;
    }

    velocity.y += GRAVITY * dt;
    position += velocity * dt;

    // 状态机
    PlayerState newState;
    if (state == PlayerState::PARRY_SUCCESS) newState = PlayerState::PARRY_SUCCESS;
    else if (parry.isBlocking())             newState = PlayerState::BLOCK;
    else if (!onGround && velocity.y < 0.f)  newState = PlayerState::JUMP;
    else if (!onGround && velocity.y > 0.f)  newState = PlayerState::FALL;
    else if (std::abs(velocity.x) > 1.f)     newState = PlayerState::RUN;
    else                                     newState = PlayerState::IDLE;

    if (newState != state) { state = newState; currentAnim().reset(); }

    currentAnim().update(dt);
    applyCurrentAnim();
    onGround = false;

    if (posture > 0.f) posture = std::max(0.f, posture - 20.f * dt);
}

void Player::onHit(const HitBox& attack) {
    if (state == PlayerState::PARRY_SUCCESS) return;
    switch (parry.judge()) {
    case ParryWindow::Result::PARRY:
        std::cout << "[PARRY!]\n";
        state = PlayerState::PARRY_SUCCESS;
        parrySuccessTimer = PARRY_SUCCESS_DURATION;
        parry.triggerParryFlash();
        return;
    case ParryWindow::Result::BLOCK:
        posture = std::min(MAX_POSTURE, posture + attack.postureDamage);
        return;
    case ParryWindow::Result::NONE:
        hp -= attack.damage;
        posture = std::min(MAX_POSTURE, posture + attack.postureDamage * 0.5f);
        state = PlayerState::HURT;
        animHurt.reset();
        velocity.x = (attack.rect.left < position.x) ? 200.f : -200.f;
        velocity.y = -200.f;
        return;
    }
}

void Player::render(sf::RenderWindow& w) {
    float sx = std::abs(sprite.getScale().x);
    sprite.setScale(facingRight ? sx : -sx, std::abs(sprite.getScale().y));
    sprite.setPosition(position);
    w.draw(sprite);

    sf::Vector2f indPos(position.x - 5.f, position.y - hitboxSize.y - 15.f);
    parry.render(w, indPos);
}

void Player::landOnGround(float groundY) {
    position.y = groundY;
    velocity.y = 0.f;
    onGround = true;
    if (state == PlayerState::HURT) state = PlayerState::IDLE;
}