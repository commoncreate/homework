#include "Boss.h"
#include "AssetManager.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "AudioManager.h"
#include "EffectSystem.h"
#include <cstdlib>   

Boss::Boss(sf::Vector2f pos) : position(pos) {
    fallbackShape.setSize({ hitboxSize.x, hitboxSize.y });
    fallbackShape.setOrigin(hitboxSize.x / 2.f, hitboxSize.y);
    fallbackShape.setFillColor(sf::Color(100, 40, 40));
    fallbackShape.setOutlineColor(sf::Color(255, 100, 100));
    fallbackShape.setOutlineThickness(3.f);
}

void Boss::loadAssets() {
    auto& AM = AssetManager::instance();

    std::cout << "=== 开始加载 Boss 贴图 ===\n";

    sf::Texture tmp;
    if (!tmp.loadFromFile("assets/sprites/boss/boss_idle.png")) {
        std::cerr << "[WARN] Boss 贴图找不到，使用方块代替\n";
        assetsLoaded = false;
        return;
    }

    const sf::Texture& tI = AM.loadTexture("boss_idle", "assets/sprites/boss/boss_idle.png");
    const sf::Texture& tR = AM.loadTexture("boss_run", "assets/sprites/boss/boss_run.png");
    const sf::Texture& tA1 = AM.loadTexture("boss_attack1", "assets/sprites/boss/boss_attack1.png");
    const sf::Texture& tA2 = AM.loadTexture("boss_attack2", "assets/sprites/boss/boss_attack2.png");
    const sf::Texture& tA3 = AM.loadTexture("boss_attack3", "assets/sprites/boss/boss_attack3.png");
    const sf::Texture& tH = AM.loadTexture("boss_hurt", "assets/sprites/boss/boss_hurt.png");

    int FH = tI.getSize().y;
    int FW = FH;

    int idleFrames = tI.getSize().x / FW;
    int runFrames = tR.getSize().x / FW;
    int attack1Frames = tA1.getSize().x / FW;
    int attack2Frames = tA2.getSize().x / FW;
    int attack3Frames = tA3.getSize().x / FW;
    int hurtFrames = tH.getSize().x / FW;

    std::cout << "[INFO] Boss 单帧尺寸: " << FW << " x " << FH << "\n";
    std::cout << "[INFO] Idle: " << idleFrames
        << ", Run: " << runFrames
        << ", Attack1: " << attack1Frames
        << ", Attack2: " << attack2Frames
        << ", Attack3: " << attack3Frames
        << ", Hurt: " << hurtFrames << "\n";
    std::cout << "=== Boss 贴图加载完成 ===\n";

    animIdle.setup(tI, idleFrames, FW, FH, 0.15f, true);
    animRun.setup(tR, runFrames, FW, FH, 0.08f, true);
    animAttack1.setup(tA1, attack1Frames, FW, FH, 0.08f, false);
    animAttack2.setup(tA2, attack2Frames, FW, FH, 0.08f, false);
    animAttack3.setup(tA3, attack3Frames, FW, FH, 0.08f, false);
    animHurt.setup(tH, hurtFrames, FW, FH, 0.10f, false);

    sprite.setOrigin(FW / 2.f, FH * 0.66f);
    sprite.setScale(1.4f, 1.4f);

    assetsLoaded = true;
    applyCurrentAnim();
}

Animation& Boss::currentAnim() {
    switch (action) {
    case BossAction::MOVE: return animRun;
    case BossAction::SLASH:
    case BossAction::DANGER_SMASH:
        if (currentAttackVariant == 1) return animAttack1;
        if (currentAttackVariant == 2) return animAttack2;
        return animAttack3;
    case BossAction::DASH: return animRun;
    case BossAction::HURT: return animHurt;
    default: return animIdle;
    }
}

void Boss::applyCurrentAnim() {
    if (assetsLoaded) currentAnim().applyTo(sprite);
}

sf::FloatRect Boss::getBounds() const {
    return { position.x - hitboxSize.x / 2.f,
             position.y - hitboxSize.y,
             hitboxSize.x, hitboxSize.y };
}

float Boss::getMoveSpeed() const {
    switch (phase) {
    case BossPhase::PHASE_1: return 80.f;
    case BossPhase::PHASE_2: return 120.f;
    case BossPhase::PHASE_3: return 180.f;
    default: return 0.f;
    }
}

void Boss::updatePhase() {
    float hpP = (float)hp / maxHp;
    BossPhase newPhase = phase;
    if (hp <= 0) newPhase = BossPhase::DEAD;
    else if (hpP <= 0.33f) newPhase = BossPhase::PHASE_3;
    else if (hpP <= 0.66f) newPhase = BossPhase::PHASE_2;
    else newPhase = BossPhase::PHASE_1;

    if (newPhase != phase) {
        phase = newPhase;
        std::cout << "[BOSS] Entering Phase " << getPhaseNumber() << "\n";
        posture.reset();
    }
}

void Boss::decideNextAction(sf::Vector2f playerPos) {
    if (phase == BossPhase::DEAD) return;
    if (action != BossAction::IDLE && action != BossAction::MOVE) return;

    float dx = playerPos.x - position.x;
    float dist = std::abs(dx);
    facingRight = dx > 0;

    int slashProb = 40, dashProb = 30, dangerProb = 10;
    if (phase == BossPhase::PHASE_2) { dashProb = 35; dangerProb = 20; }
    if (phase == BossPhase::PHASE_3) { slashProb = 30; dashProb = 40; dangerProb = 30; }

    int roll = std::rand() % 100;

    if (dist < 120.f && roll < slashProb) startSlash();
    else if (dist < 400.f && roll < slashProb + dashProb) startDash(playerPos);
    else if (roll < slashProb + dashProb + dangerProb) startDangerSmash();
    else { action = BossAction::MOVE; actionTimer = 0.5f; }
}

void Boss::startSlash() {
    action = BossAction::SLASH;
    actionTimer = 0.8f;
    velocity.x = 0.f;
    currentAttackVariant = 1 + (std::rand() % 3);
    if (assetsLoaded) {
        animAttack1.reset();
        animAttack2.reset();
        animAttack3.reset();
    }
    std::cout << "[BOSS] Slash variant " << currentAttackVariant << "!\n";
}

void Boss::startDash(sf::Vector2f playerPos) {
    action = BossAction::DASH;
    actionTimer = 0.7f;
    float dir = (playerPos.x > position.x) ? 1.f : -1.f;
    velocity.x = dir * 500.f;
    std::cout << "[BOSS] Dash!\n";
}

void Boss::startDangerSmash() {
    action = BossAction::DANGER_SMASH;
    actionTimer = 1.3f;
    velocity.x = 0.f;
    danger.trigger(0.9f);
    currentAttackVariant = 3;
    if (assetsLoaded) animAttack3.reset();
    AudioManager::instance().playSound("danger", 100.f);
    std::cout << "[BOSS] DANGER SMASH!\n";
}

void Boss::spawnSlashHitbox() {
    float dir = facingRight ? 1.f : -1.f;
    atkHitbox.rect = sf::FloatRect(
        position.x + dir * 20.f - (facingRight ? 0.f : 90.f),
        position.y - 80.f,
        90.f, 70.f);
    atkHitbox.damage = 15;
    atkHitbox.postureDamage = 15.f;
    atkHitbox.active = true;
    atkHitbox.lifeTime = 0.2f;
    atkHitbox.ownerId = 0;
}

void Boss::spawnDashHitbox() {
    atkHitbox.rect = getBounds();
    atkHitbox.damage = 20;
    atkHitbox.postureDamage = 18.f;
    atkHitbox.active = true;
    atkHitbox.lifeTime = 0.4f;
    atkHitbox.ownerId = 0;
}

void Boss::spawnSmashHitbox() {
    atkHitbox.rect = sf::FloatRect(
        position.x - 150.f,
        position.y - 100.f,
        300.f, 100.f);
    atkHitbox.damage = 30;
    atkHitbox.postureDamage = 0.f;
    atkHitbox.active = true;
    atkHitbox.lifeTime = 0.3f;
    atkHitbox.ownerId = 0;
}

void Boss::updateAction(float dt, sf::Vector2f playerPos) {
    actionTimer -= dt;

    switch (action) {
    case BossAction::MOVE: {
        float dir = (playerPos.x > position.x) ? 1.f : -1.f;
        velocity.x = dir * getMoveSpeed();
        facingRight = dir > 0;
        if (actionTimer <= 0.f) { action = BossAction::IDLE; velocity.x = 0.f; }
        break;
    }
    case BossAction::SLASH: {
        float elapsed = 0.8f - actionTimer;
        if (elapsed > 0.4f && elapsed < 0.6f && !atkHitbox.active) {
            spawnSlashHitbox();

            sf::Vector2f fxPos = position;
            fxPos.y -= hitboxSize.y * 0.5f;
            EffectSystem::instance().trigger(EffectType::BOSS_SLASH, fxPos, facingRight);
        }
        if (elapsed > 0.6f) atkHitbox.active = false;
        if (actionTimer <= 0.f) action = BossAction::IDLE;
        break;
    }
    case BossAction::SWEEP: {
        float elapsed = 0.6f - actionTimer;

        if (elapsed > 0.2f && elapsed < 0.4f && !atkHitbox.active) {
            atkHitbox.rect = sf::FloatRect(
                position.x - 180.f, position.y - hitboxSize.y, 360.f, hitboxSize.y + 20.f
            );
            atkHitbox.damage = 35;
            atkHitbox.postureDamage = 25.f;
            atkHitbox.active = true;
            atkHitbox.ownerId = 0;

            sf::Vector2f fxPos = position;
            fxPos.y -= hitboxSize.y * 0.5f;
            EffectSystem::instance().trigger(EffectType::BOSS_SWEEP, fxPos, facingRight);
        }
        if (elapsed > 0.4f) atkHitbox.active = false;

        if (actionTimer <= 0.f) {
            action = BossAction::IDLE;
            isSuperArmor = false;  
            std::cout << "[BOSS] Sweep ended\n";
        }
        break;
    }
    case BossAction::DASH: {
        if (!atkHitbox.active) spawnDashHitbox();
        atkHitbox.rect = getBounds();

        dashTrailTimer -= dt;
        if (dashTrailTimer <= 0.f) {
            sf::Vector2f fxPos = position;
            fxPos.y -= hitboxSize.y * 0.5f;
            EffectSystem::instance().trigger(EffectType::BOSS_DASH_TRAIL, fxPos, facingRight);
            dashTrailTimer = 0.08f;
        }

        if (actionTimer <= 0.f) {
            action = BossAction::IDLE;
            velocity.x = 0.f;
            atkHitbox.active = false;
        }
        break;
    }
    case BossAction::DANGER_SMASH: {
        float elapsed = 1.3f - actionTimer;
        if (elapsed > 0.9f && elapsed < 1.2f && !atkHitbox.active) {
            spawnSmashHitbox();

            sf::Vector2f fxPos = position;
            fxPos.y -= hitboxSize.y * 0.5f;
            EffectSystem::instance().trigger(EffectType::BOSS_DANGER_SMASH, fxPos, true);
        }
        if (elapsed > 1.2f) atkHitbox.active = false;
        if (actionTimer <= 0.f) action = BossAction::IDLE;
        break;
    }
    case BossAction::STUNNED:
    case BossAction::HURT:
        velocity.x = 0.f;
        if (actionTimer <= 0.f) action = BossAction::IDLE;
        break;
    default: break;
    }

    atkHitbox.update(dt);
}

void Boss::update(float dt, sf::Vector2f playerPos, float arenaLeft, float arenaRight) {
    if (kHitResetTimer > 0.f) {
        kHitResetTimer -= dt;
        if (kHitResetTimer <= 0.f) {
            if (kHitCounter > 0) {
                std::cout << "[BOSS] K combo reset (timeout)\n";
            }
            kHitCounter = 0;
        }
    }


    for (auto& p : counterProjectiles) p.update(dt);
    counterProjectiles.erase(
        std::remove_if(counterProjectiles.begin(), counterProjectiles.end(),
            [](const BossProjectile& p) { return !p.isAlive(); }),
        counterProjectiles.end());
 
    if (phase == BossPhase::DEAD) return;

    updatePhase();

    bool wasBroken = posture.isBroken();
    posture.update(dt, (float)hp / maxHp);    
    bool nowBroken = posture.isBroken();

    if (wasBroken && !nowBroken) {
        staggerBonus = true;
        staggerBonusTimer = 5.f;
        std::cout << "[BOSS] Stagger WINDOW OPEN! Next hit x3 damage (5s)\n";
    }

    if (staggerBonusTimer > 0.f) {
        staggerBonusTimer -= dt;
        if (staggerBonusTimer <= 0.f) {
            staggerBonus = false;
        }
    }

    danger.update(dt);

    aiDecisionTimer -= dt;
    if (aiDecisionTimer <= 0.f) {
        decideNextAction(playerPos);
        aiDecisionTimer = 1.2f + (std::rand() % 100) / 100.f;
    }

    updateAction(dt, playerPos);

    if (posture.isBroken() && action != BossAction::STUNNED) {
        action = BossAction::STUNNED;
        actionTimer = 2.5f;
        velocity.x = 0.f;
        atkHitbox.active = false;
        std::cout << "[BOSS] POSTURE BROKEN!\n";
    }

    position += velocity * dt;

    float half = hitboxSize.x / 2.f;
    if (position.x < arenaLeft + half) { position.x = arenaLeft + half;  velocity.x = 0.f; }
    if (position.x > arenaRight - half) { position.x = arenaRight - half; velocity.x = 0.f; }

    if (assetsLoaded) {
        if (action != prevAction) {
            currentAnim().reset();
            prevAction = action;
        }
        currentAnim().update(dt);
        applyCurrentAnim();
    }
}
void Boss::counterAttack() {
    sf::Vector2f spawn = position;
    spawn.y -= hitboxSize.y * 0.5f;
    spawn.x += facingRight ? 40.f : -40.f;

    counterProjectiles.emplace_back(spawn, facingRight);

    AudioManager::instance().playSound("danger", 80.f);
    std::cout << "[BOSS] COUNTER! Red moon flying!\n";
}
void Boss::triggerSweep() {
    action = BossAction::SWEEP;
    actionTimer = 0.6f;       
    isSuperArmor = true;       
    velocity.x = 0.f;

    if (assetsLoaded) {
        currentAttackVariant = 3;
        animAttack3.reset();
    }

    AudioManager::instance().playSound("danger", 100.f);
    std::cout << "[BOSS] *** RAGE SWEEP! SUPER ARMOR! ***\n";
}

void Boss::takeHit(const HitBox& atk, bool isSkill) {
    if (atk.ownerId != 1) return;
    if (phase == BossPhase::DEAD) return;

    if (action == BossAction::SWEEP && isSuperArmor) {
        hp -= atk.damage / 2;   
        posture.addPosture(atk.postureDamage * 0.3f);
        AudioManager::instance().playSound("hit", 60.f);
        std::cout << "[BOSS] Super armor hit! HP=" << hp << "\n";
        return;
    }

    if (posture.isBroken()) {
        hp = 0;
        std::cout << "[BOSS] Executed!\n";
        return;
    }

    int finalDamage = atk.damage;
    if (staggerBonus) {
        finalDamage = atk.damage * 3;
        staggerBonus = false;
        std::cout << "[BOSS] STAGGER BONUS! Damage x3: " << finalDamage << "\n";
    }

    hp -= finalDamage;
    posture.addPosture(atk.postureDamage);
    action = BossAction::HURT;
    actionTimer = 0.25f;
    if (assetsLoaded) animHurt.reset();

    AudioManager::instance().playSound("hit", 90.f);

    if (isSkill) {
        float r = (std::rand() % 100) / 100.f;
        if (r < COUNTER_CHANCE) {
            counterAttack();  
        }
    }
    else {
        kHitCounter++;
        kHitResetTimer = K_HIT_RESET;
        std::cout << "[BOSS] K combo: " << kHitCounter << "/" << K_HIT_THRESHOLD << "\n";

        if (kHitCounter >= K_HIT_THRESHOLD) {
            kHitCounter = 0;   

            float r = (std::rand() % 100) / 100.f;
            if (r < SWEEP_CHANCE) {
                triggerSweep();
            }
        }
    }

    std::cout << "[BOSS] HP=" << hp << " Posture=" << posture.getCurrent() << "\n";
}

/*void Boss::takeHit(const HitBox& atk, bool isSkill) {
    if (atk.ownerId != 1) return;
    if (phase == BossPhase::DEAD) return;

    if (posture.isBroken()) {
        hp = 0;
        std::cout << "[BOSS] Executed!\n";
        return;
    }
    int finalDamage = atk.damage;
    if (staggerBonus) {
        finalDamage = atk.damage * 3;   
        staggerBonus = false;          
        std::cout << "[BOSS] STAGGER BONUS! Damage x3: " << finalDamage << "\n";
    }
    hp -= finalDamage;
    posture.addPosture(atk.postureDamage);
    action = BossAction::HURT;
    actionTimer = 0.25f;
    if (assetsLoaded) animHurt.reset();
    AudioManager::instance().playSound("hit", 90.f);
    std::cout << "[BOSS] HP=" << hp << " Posture=" << posture.getCurrent() << "\n";
}
*/
void Boss::onParried() {
    posture.addPosture(50.f);
    std::cout << "[BOSS] Parried! Posture +50\n";
}

void Boss::render(sf::RenderWindow& w) {
    if (phase == BossPhase::DEAD) return;
    if (assetsLoaded) {
        float sx = std::abs(sprite.getScale().x);
        sprite.setScale(facingRight ? sx : -sx, std::abs(sprite.getScale().y));
        sprite.setPosition(position);
        if (staggerBonus) {
            int blink = (int)(staggerBonusTimer * 5) % 2;
            sprite.setColor(blink ? sf::Color(255, 100, 100) : sf::Color(255, 180, 180));
        }
        else {
            sprite.setColor(sf::Color::White);
        }
        w.draw(sprite);
    }
    else {
        switch (action) {
        case BossAction::SLASH: fallbackShape.setFillColor(sf::Color(200, 80, 40)); break;
        case BossAction::DASH:  fallbackShape.setFillColor(sf::Color(255, 120, 40)); break;
        case BossAction::DANGER_SMASH: {
            int flash = (int)(actionTimer * 10) % 2;
            fallbackShape.setFillColor(flash ? sf::Color(255, 0, 0) : sf::Color(150, 0, 0));
            break;
        }
        case BossAction::STUNNED: fallbackShape.setFillColor(sf::Color(255, 255, 100)); break;
        case BossAction::HURT:    fallbackShape.setFillColor(sf::Color(255, 180, 180)); break;
        default:                  fallbackShape.setFillColor(sf::Color(100, 40, 40)); break;
        }
        fallbackShape.setPosition(position);
        w.draw(fallbackShape);
    }
    for (auto& p : counterProjectiles) p.render(w);
    /*if (atkHitbox.active) {
        sf::RectangleShape r({ atkHitbox.rect.width, atkHitbox.rect.height });
        r.setPosition(atkHitbox.rect.left, atkHitbox.rect.top);
        r.setFillColor(sf::Color(255, 0, 0, 100));
        w.draw(r);
    }*/
    sf::Vector2f barPos(position.x - 40.f, position.y - hitboxSize.y - 20.f);
    posture.render(w, barPos, 80.f);
}

void Boss::renderUI(sf::RenderWindow& w, sf::Vector2f viewCenter) {
    sf::Vector2f screenPos(
        640.f + (position.x - viewCenter.x),
        360.f + (position.y - 200.f - viewCenter.y)
    );
    danger.render(w, screenPos);
}