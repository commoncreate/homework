#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "PostureSystem.h"
#include "DangerWarning.h"
#include "Animation.h"
#include <vector>
#include "BossProjectile.h"

enum class BossPhase { PHASE_1, PHASE_2, PHASE_3, DEAD };
enum class BossAction { IDLE, MOVE, SLASH, DASH, DANGER_SMASH, STUNNED, HURT, DEAD , SWEEP
};

class Boss {
public:
    Boss(sf::Vector2f pos);
    void loadAssets();

    void update(float dt, sf::Vector2f playerPos, float arenaLeft, float arenaRight);
    void render(sf::RenderWindow& w);
    void renderUI(sf::RenderWindow& w, sf::Vector2f viewCenter);

    void takeHit(const HitBox& atk, bool isSkill = false);
    void onParried();



    bool isDead() const { return phase == BossPhase::DEAD; }
    bool canBeExecuted() const { return posture.isBroken() && !isDead(); }

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    HitBox& getAttackHitBox() { return atkHitbox; }

    std::vector<BossProjectile>& getCounterProjectiles() { return counterProjectiles; }

    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }
    float getPosturePercent() const { return posture.getPercent(); }
    int getPhaseNumber() const {
        if (phase == BossPhase::PHASE_1) return 1;
        if (phase == BossPhase::PHASE_2) return 2;
        if (phase == BossPhase::PHASE_3) return 3;
        return 0;
    }

private:
    sf::Vector2f position, velocity;
    sf::Vector2f hitboxSize{ 65.f, 110.f };
    bool facingRight = false;

    int hp = 300, maxHp = 300;
    PostureSystem posture{ 150.f };
    bool staggerBonus = false;     
    float staggerBonusTimer = 0.f;
    const float STAGGER_BONUS_DURATION = 5.f;

    DangerWarning danger;

    BossPhase phase = BossPhase::PHASE_1;
    BossAction action = BossAction::IDLE;
    BossAction prevAction = BossAction::IDLE;
    float actionTimer = 0.f;
    float aiDecisionTimer = 1.5f;

    HitBox atkHitbox;

    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;

    Animation animIdle, animRun, animHurt;
    Animation animAttack1, animAttack2, animAttack3;
    int currentAttackVariant = 1;   
    bool assetsLoaded = false;

    int FW = 200, FH = 200;
    int framesIdle = 8;
    int framesRun = 8;
    int framesAttack1 = 4;
    int framesAttack2 = 4;
    int framesAttack3 = 4;
    int framesHurt = 3;
    float originY = 170.f;   

    void decideNextAction(sf::Vector2f playerPos);
    void updateAction(float dt, sf::Vector2f playerPos);
    void startSlash();
    void startDash(sf::Vector2f playerPos);
    void startDangerSmash();

    void spawnSlashHitbox();
    void spawnDashHitbox();
    void spawnSmashHitbox();
    void updatePhase();
    void counterAttack();     
    void triggerSweep();


    Animation& currentAnim();
    void applyCurrentAnim();
    float dashTrailTimer = 0.f;
    float getMoveSpeed() const;

    std::vector<BossProjectile> counterProjectiles;   
    const float COUNTER_CHANCE = 0.35f;               


    int kHitCounter = 0;              
    float kHitResetTimer = 0.f;      
    const float K_HIT_RESET = 3.0f;   
    const int K_HIT_THRESHOLD = 4;    
    const float SWEEP_CHANCE = 0.5f;  

    bool isSuperArmor = false;        
};