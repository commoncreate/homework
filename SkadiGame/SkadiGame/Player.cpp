#include "Player.h"
#include "AssetManager.h"
#include "Boss.h"
#include "HitStop.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "AudioManager.h"
#include "EffectSystem.h"

Player::Player() { position = { 200.f, 400.f }; }

void Player::loadAssets() {
    auto& AM = AssetManager::instance();
    const int FW = 180, FH = 180;

    const sf::Texture& tIdle = AM.loadTexture("skadi_idle", "assets/sprites/skadi_idle.png");
    const sf::Texture& tRun = AM.loadTexture("skadi_run", "assets/sprites/skadi_run.png");
    const sf::Texture& tJump = AM.loadTexture("skadi_jump", "assets/sprites/skadi_jump.png");
    const sf::Texture& tFall = AM.loadTexture("skadi_fall", "assets/sprites/skadi_fall.png");
    const sf::Texture& tBlock = AM.loadTexture("skadi_block", "assets/sprites/skadi_block.png");
    const sf::Texture& tHurt = AM.loadTexture("skadi_hurt", "assets/sprites/skadi_hurt.png");
    const sf::Texture& tAtk1 = AM.loadTexture("skadi_attack1", "assets/sprites/skadi_attack1.png");
    const sf::Texture& tAtk2 = AM.loadTexture("skadi_attack2", "assets/sprites/skadi_attack2.png");

    animIdle.setup(tIdle, 10, FW, FH, 0.10f, true);
    animRun.setup(tRun, 10, FW, FH, 0.08f, true);
    animJump.setup(tJump, 3, FW, FH, 0.10f, false);
    animFall.setup(tFall, 3, FW, FH, 0.10f, false);
    animBlock.setup(tBlock, 5, FW, FH, 0.08f, false);
    animHurt.setup(tHurt, 3, FW, FH, 0.10f, false);
    animAttack1.setup(tAtk1, 7, FW, FH, 0.05f, false);
    animAttack2.setup(tAtk2, 7, FW, FH, 0.05f, false);
    sprite.setOrigin(FW / 2.f, 118.f);
    sprite.setScale(1.0f, 1.0f);

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
    case PlayerState::ATTACK:                             
        return (comboIndex == 2) ? animAttack2 : animAttack1;
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
    if (attackSys.isAttacking() && !attackSys.canCancel()) {
        velocity.x = 0.f;   
        return;
    }

    velocity.x = 0.f;
    float mul = parry.isBlocking() ? 0.4f : 1.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { velocity.x = -MOVE_SPEED * mul; facingRight = false; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { velocity.x = MOVE_SPEED * mul; facingRight = true; }

    bool spNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spNow && !spacePrev && !parry.isBlocking()) {
        if (onGround) { velocity.y = JUMP_FORCE; onGround = false; canDoubleJump = true; }
        else if (canDoubleJump) { velocity.y = JUMP_FORCE * 0.85f; canDoubleJump = false; }
    }
    spacePrev = spNow;

    bool jNow = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
    if (jNow && !jPrev) parry.startBlock();
    else if (!jNow && jPrev) parry.stopBlock();
    jPrev = jNow;

    bool kNow = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
    if (kNow && !kPrev && !parry.isBlocking() ) {
        if (!attackSys.isAttacking()) {
            attackSys.startAttack(position, facingRight);
            comboIndex = 1;
            comboWindow = 1.5f;    
        }
        else if (comboIndex == 1 && comboWindow > 0.f) {
            attackSys.startAttack(position, facingRight);
            comboIndex = 2;
            comboWindow = 0.f;     
            animAttack2.reset();   
        }
    }
    bool hNow = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
    if (hNow && !hPrev && !parry.isBlocking()
        && !attackSys.isAttacking() && skillCD_H <= 0.f) {

        attackSys.startAttack(position, facingRight);
        AudioManager::instance().playSound("slash", 70.f);

        sf::Vector2f spawnPos = position;
        spawnPos.y -= hitboxSize.y * 0.5f;
        spawnPos.x += facingRight ? 40.f : -40.f;
        projectiles.emplace_back(spawnPos, facingRight);

        skillCD_H = 0.5f;   
        std::cout << "[SKILL] Moon Slash!\n";
    }
    hPrev = hNow;


    bool yNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Y);
    if (yNow && !yPrev && !parry.isBlocking() && skillCD_Y <= 0.f) {
        float dir = facingRight ? 1.f : -1.f;
        sf::Vector2f fxPos = position;
        fxPos.x += facingRight ? 120.f : -120.f;
        fxPos.y -= hitboxSize.y * 0.5f;
        EffectSystem::instance().trigger(EffectType::CROSS_SLASH, fxPos, facingRight);
        AudioManager::instance().playSound("slash", 100.f);
        skillHitbox_Y.rect = sf::FloatRect(
            fxPos.x - 140.f,          
            fxPos.y - 140.f,
            280.f, 280.f
        );
        skillHitbox_Y.damage = 30;
        skillHitbox_Y.postureDamage = 25.f;
        skillHitbox_Y.active = true;
        skillHitbox_Y.lifeTime = 0.4f;   
        skillHitbox_Y.ownerId = 1;
        skillHitbox.isSkill = true;

        AudioManager::instance().playSound("slash", 100.f);

        attackSys.startAttack(position, facingRight);

        skillCD_Y = 1.0f;   
        std::cout << "[SKILL] Cross Slash!\n";
    }
    yPrev = yNow;


    bool uNow = sf::Keyboard::isKeyPressed(sf::Keyboard::U);
    if (uNow && !uPrev && !parry.isBlocking() && skillCD_U <= 0.f) {
        float dir = facingRight ? 1.f : -1.f;
        for (int i = 0; i < 5; ++i) {
            sf::Vector2f icePos;
            icePos.x = position.x + dir * (80.f + i * 60.f);
            icePos.y = position.y;   
            EffectSystem::instance().trigger(EffectType::ICE_SPIKE, icePos, facingRight);
        }
        AudioManager::instance().playSound("execute", 90.f);

        float firstX = position.x + dir * 80.f;
        float lastX = position.x + dir * (80.f + 4 * 60.f);
        float minX = std::min(firstX, lastX) - 40.f;
        float maxX = std::max(firstX, lastX) + 40.f;

        skillHitbox_U.rect = sf::FloatRect(
            minX,
            position.y - 100.f,       
            maxX - minX, 120.f
        );
        skillHitbox_U.damage = 40;
        skillHitbox_U.postureDamage = 35.f;
        skillHitbox_U.active = true;
        skillHitbox_U.lifeTime = 0.7f;   
        skillHitbox_U.ownerId = 1;
        skillHitbox.isSkill = true;

        AudioManager::instance().playSound("execute", 90.f);   

        skillCD_U = 2.0f;  
        std::cout << "[SKILL] Ice Spike!\n";
    }
    uPrev = uNow;

    /*bool hNow = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
    if (hNow && !kPrev && !parry.isBlocking() && !attackSys.isAttacking()) {
        attackSys.startAttack(position, facingRight);
        AudioManager::instance().playSound("slash", 70.f);
        sf::Vector2f fxPos = position;
        fxPos.y -= hitboxSize.y * 0.5f;
        EffectSystem::instance().trigger(EffectType::PLAYER_SLASH, fxPos, facingRight);

    }
    AudioManager::instance().playSound("slash", 70.f);
    kPrev = kNow;*/
}

void Player::update(float dt) {

    if (skillCD_H > 0.f) skillCD_H -= dt;
    if (skillCD_Y > 0.f) skillCD_Y -= dt;
    if (skillCD_U > 0.f) skillCD_U -= dt;

    skillHitbox_Y.update(dt);
    skillHitbox_U.update(dt);
    skillHitbox.update(dt);

    for (auto& p : projectiles) p.update(dt);
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.isAlive(); }),
        projectiles.end());



    parry.update(dt);
    attackSys.update(dt, position);

    if (comboWindow > 0.f) comboWindow -= dt;
    if (!attackSys.isAttacking() && comboIndex != 0) {
        comboIndex = 0;    
    }

    if (state == PlayerState::PARRY_SUCCESS) {
        parrySuccessTimer -= dt;
        velocity.x = 0.f;
        if (parrySuccessTimer <= 0.f) state = PlayerState::IDLE;
    }

    velocity.y += GRAVITY * dt;
    position += velocity * dt;

    PlayerState ns;
    if (state == PlayerState::PARRY_SUCCESS) ns = PlayerState::PARRY_SUCCESS;
    else if (attackSys.isAttacking()) ns = PlayerState::ATTACK;
    else if (parry.isBlocking()) ns = PlayerState::BLOCK;
    else if (!onGround && velocity.y < 0.f) ns = PlayerState::JUMP;
    else if (!onGround && velocity.y > 0.f) ns = PlayerState::FALL;
    else if (std::abs(velocity.x) > 1.f) ns = PlayerState::RUN;
    else ns = PlayerState::IDLE;


    if (ns != state) { state = ns; currentAnim().reset(); }
    currentAnim().update(dt);
    applyCurrentAnim();
    onGround = false;
    if (posture > 0.f) posture = std::max(0.f, posture - 20.f * dt);
}

void Player::onHit(const HitBox& atk) {
    if (state == PlayerState::PARRY_SUCCESS) return;
    if (atk.isSkill && atk.ownerId == 0) {
        std::cout << "[HURT] Counter moon hit! (unblockable)\n";
        hp -= atk.damage;
        posture = std::min(MAX_POSTURE, posture + atk.postureDamage);
        state = PlayerState::HURT;
        animHurt.reset();
        velocity.x = (atk.rect.left < position.x) ? 250.f : -250.f;
        velocity.y = -300.f;
        HitStop::instance().freeze(0.1f);
        AudioManager::instance().playSound("hurt", 100.f);
        return;
    }



    switch (parry.judge()) {
    case ParryWindow::Result::PARRY:
        std::cout << "[PARRY!]\n";
        state = PlayerState::PARRY_SUCCESS;
        parrySuccessTimer = PARRY_SUCCESS_DURATION;
        parry.triggerParryFlash();
        HitStop::instance().freeze(0.1f);
        AudioManager::instance().playSound("parry", 100.f);
        return;
    case ParryWindow::Result::BLOCK:
        posture = std::min(MAX_POSTURE, posture + atk.postureDamage);
        AudioManager::instance().playSound("block", 80.f);
        return;
    case ParryWindow::Result::NONE:
        hp -= atk.damage;
        posture = std::min(MAX_POSTURE, posture + atk.postureDamage * 0.5f);
        state = PlayerState::HURT;
        animHurt.reset();
        velocity.x = (atk.rect.left < position.x) ? 200.f : -200.f;
        velocity.y = -200.f;
        HitStop::instance().freeze(0.08f);
        AudioManager::instance().playSound("hurt", 80.f);
        return;
    }
}

void Player::tryExecute(Boss& boss) {
    bool fNow = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
    if (fNow && !fPrev && boss.canBeExecuted()) {
        float dist = std::abs(boss.getPosition().x - position.x);
        if (dist < 150.f) {
            HitBox exec;
            exec.damage = 9999;
            exec.ownerId = 1;
            boss.takeHit(exec);
            HitStop::instance().freeze(0.4f);
            AudioManager::instance().playSound("execute", 120.f);
            std::cout << "[PLAYER] EXECUTE!\n";
        }
    }
    fPrev = fNow;
}

void Player::render(sf::RenderWindow& w) {
    float sx = std::abs(sprite.getScale().x);
    sprite.setScale(facingRight ? sx : -sx, std::abs(sprite.getScale().y));
    sprite.setPosition(position);
    w.draw(sprite);
    for (auto& p : projectiles) p.render(w);
    attackSys.render(w);

    sf::Vector2f ind(position.x - 5.f, position.y - hitboxSize.y - 15.f);
    parry.render(w, ind);
}

void Player::landOnGround(float y) {
    position.y = y;
    velocity.y = 0.f;
    onGround = true;
    if (state == PlayerState::HURT) state = PlayerState::IDLE;
}