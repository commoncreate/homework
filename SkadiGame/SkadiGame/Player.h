#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "ParryWindow.h"
#include "Animation.h"
#include "PlayerAttack.h"
#include "SlashEffect.h"
#include "Projectile.h"
#include <vector>

class Boss;  

enum class PlayerState {
    IDLE, RUN, JUMP, FALL,
    BLOCK, PARRY_SUCCESS, HURT, DEAD, ATTACK
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
    bool  isFacingRight() const { return facingRight; }
    bool  isDead() const { return hp <= 0; }

    // expose invincibility check to other systems (was private)
    bool isInvincible() const { return state == PlayerState::PARRY_SUCCESS; }

    HitBox& getSkillHitbox_Y() { return skillHitbox_Y; }
    HitBox& getSkillHitbox_U() { return skillHitbox_U; }
    HitBox& getSkillHitBox() { return skillHitbox; }
    HitBox skillHitbox;
    PlayerAttack& getAttack() { return attackSys; }

    // Access to active projectiles
    std::vector<Projectile>& getProjectiles() { return projectiles; }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }

    void tryExecute(Boss& boss);
    void reset() {
        hp = maxHp;          
        posture = 0.f;
        velocity = { 0.f, 0.f };
        state = PlayerState::IDLE;
        parrySuccessTimer = 0.f;
        onGround = false;
        canDoubleJump = false;
        facingRight = true;
        spacePrev = jPrev = kPrev = fPrev = false;
    }
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f hitboxSize{ 55.f, 100.f };
    int comboIndex = 0;          
    float comboWindow = 0.f;

    Animation animIdle, animRun, animJump, animFall, animBlock, animHurt;
    Animation animAttack1, animAttack2;
    bool facingRight = true;
    const float MOVE_SPEED = 300.f;
    const float JUMP_FORCE = -550.f;
    const float GRAVITY = 1400.f;
    bool onGround = false;
    bool canDoubleJump = false;

    HitBox skillHitbox_Y;     
    HitBox skillHitbox_U;


    float skillCD_H = 0.f;    
    float skillCD_Y = 0.f;    
    float skillCD_U = 0.f;    

    bool hPrev = false;
    bool yPrev = false;
    bool uPrev = false;

    std::vector<Projectile> projectiles;

    PlayerState state = PlayerState::IDLE;
    ParryWindow parry{ 0.15f, 0.4f };
    PlayerAttack attackSys;

    float parrySuccessTimer = 0.f;
    const float PARRY_SUCCESS_DURATION = 0.3f;

    int hp = 100, maxHp = 100;
    float posture = 0.f;
    const float MAX_POSTURE = 100.f;

    bool spacePrev = false, jPrev = false, kPrev = false, fPrev = false;

    Animation& currentAnim();
    void applyCurrentAnim();
};