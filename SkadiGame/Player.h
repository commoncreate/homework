#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "ParryWindow.h"
#include "Animation.h"

enum class PlayerState {
    IDLE, RUN, JUMP, FALL,
    BLOCK, PARRY_SUCCESS, HURT, DEAD
};

class Player {
public:
    Player();
    void loadAssets();

    void handleInput(float dt);
    void update(float dt);
    void render(sf::RenderWindow& w);

    void onHit(const HitBox& attack);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    void setPosition(sf::Vector2f p) { position = p; velocity = { 0.f, 0.f }; }
    void landOnGround(float groundY);
    void pushLeftTo(float x) { position.x = x; if (velocity.x > 0.f) velocity.x = 0.f; }
    void pushRightTo(float x) { position.x = x; if (velocity.x < 0.f) velocity.x = 0.f; }
    void hitCeiling(float y) { position.y = y + hitboxSize.y; if (velocity.y < 0.f) velocity.y = 0.f; }

    int   getHP() const { return hp; }
    float getPosture() const { return posture; }
    float getParryFlashAlpha() const { return parry.getFlashAlpha(); }

private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f hitboxSize{ 40.f, 70.f };

    Animation animIdle, animRun, animJump, animFall, animBlock, animHurt;
    bool facingRight = true;

    const float MOVE_SPEED = 300.f;
    const float JUMP_FORCE = -550.f;
    const float GRAVITY = 1400.f;
    bool onGround = false;
    bool canDoubleJump = false;

    PlayerState state = PlayerState::IDLE;
    ParryWindow parry{ 0.15f, 0.4f };
    float parrySuccessTimer = 0.f;
    const float PARRY_SUCCESS_DURATION = 0.3f;

    int hp = 100, maxHp = 100;
    float posture = 0.f;
    const float MAX_POSTURE = 100.f;

    bool spacePrev = false;
    bool jPrev = false;

    Animation& currentAnim();
    void applyCurrentAnim();
};