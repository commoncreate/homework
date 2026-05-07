#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

enum class EffectType {
    PLAYER_SLASH,       
    BOSS_SLASH,         
    BOSS_DASH_TRAIL,    
    BOSS_DANGER_SMASH,
    CROSS_SLASH,    
    ICE_SPIKE,
    BOSS_SWEEP
};

struct Effect {
    EffectType type;
    sf::Vector2f position;
    bool facingRight = true;
    float timer = 0.f;
    float duration = 0.3f;
};

class EffectSystem {
public:
    static EffectSystem& instance() {
        static EffectSystem inst;
        return inst;
    }

    void trigger(EffectType t, sf::Vector2f pos, bool facingRight = true) {
        Effect e;
        e.type = t;
        e.position = pos;
        e.facingRight = facingRight;

        switch (t) {
        case EffectType::PLAYER_SLASH:      e.duration = 0.30f; break;
        case EffectType::BOSS_SLASH:        e.duration = 0.40f; break;
        case EffectType::BOSS_DASH_TRAIL:   e.duration = 0.20f; break;
        case EffectType::BOSS_DANGER_SMASH: e.duration = 0.50f; break;
        case EffectType::CROSS_SLASH:       e.duration = 0.45f; break;  
        case EffectType::ICE_SPIKE:         e.duration = 0.60f; break;
        case EffectType::BOSS_SWEEP:        e.duration = 0.50f; break;
        }
        e.timer = e.duration;
        effects.push_back(e);
    }

    void update(float dt) {
        for (auto& e : effects) e.timer -= dt;
        effects.erase(
            std::remove_if(effects.begin(), effects.end(),
                [](const Effect& e) { return e.timer <= 0.f; }),
            effects.end());
    }

    void render(sf::RenderWindow& w) {
        for (auto& e : effects) {
            float t = 1.f - e.timer / e.duration;   
            switch (e.type) {
            case EffectType::PLAYER_SLASH:      drawPlayerSlash(w, e, t);      break;
            case EffectType::BOSS_SLASH:        drawBossSlash(w, e, t);        break;
            case EffectType::BOSS_DASH_TRAIL:   drawBossDashTrail(w, e, t);    break;
            case EffectType::BOSS_DANGER_SMASH: drawBossDangerSmash(w, e, t);  break;
            case EffectType::CROSS_SLASH:       drawCrossSlash(w, e, t);       break;  
            case EffectType::ICE_SPIKE:         drawIceSpike(w, e, t);         break;
            case EffectType::BOSS_SWEEP:        drawBossSweep(w, e, t);        break;
            }
        }
    }

    void clear() { effects.clear(); }

private:
    std::vector<Effect> effects;

    void drawPlayerSlash(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        float dir = e.facingRight ? 1.f : -1.f;

        sf::Vector2f center = e.position;
        center.x += dir * 60.f;  

        float length = 140.f + t * 40.f;   
        float slashWidth = 8.f * (1.f - t);  

        sf::ConvexShape outer;
        outer.setPointCount(4);
        outer.setPoint(0, { -slashWidth - 4.f, -length / 2.f });
        outer.setPoint(1, { slashWidth + 4.f, -length / 2.f });
        outer.setPoint(2, { slashWidth + 4.f,  length / 2.f });
        outer.setPoint(3, { -slashWidth - 4.f, length / 2.f });
        outer.setFillColor(sf::Color(100, 180, 255, (sf::Uint8)(alpha * 0.6f)));
        outer.setPosition(center);
        outer.setRotation(dir > 0 ? 15.f : -15.f);   
        w.draw(outer);

        sf::ConvexShape inner;
        inner.setPointCount(4);
        inner.setPoint(0, { -slashWidth, -length / 2.f });
        inner.setPoint(1, { slashWidth, -length / 2.f });
        inner.setPoint(2, { slashWidth,  length / 2.f });
        inner.setPoint(3, { -slashWidth, length / 2.f });
        inner.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)alpha));
        inner.setPosition(center);
        inner.setRotation(dir > 0 ? 15.f : -15.f);
        w.draw(inner);

        for (int i = 0; i < 5; ++i) {
            float sprayAngle = ((std::rand() % 60) - 30) * 0.017f;   
            float d = t * 80.f + i * 10.f;
            sf::CircleShape spark(2.f + (std::rand() % 3));
            spark.setPosition(
                center.x + std::cos(sprayAngle) * d * dir,
                center.y + length / 2.f + std::sin(sprayAngle) * d
            );
            spark.setFillColor(sf::Color(200, 230, 255, (sf::Uint8)(alpha * 0.7f)));
            w.draw(spark);
        }
    }

    void drawBossSlash(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        float dir = e.facingRight ? 1.f : -1.f;

        sf::Vector2f center = e.position;
        center.x += dir * 70.f;

        float length = 180.f;
        float slashWidth = 10.f;

        float expandScale = 0.5f + t * 0.8f;
        sf::ConvexShape outer;
        outer.setPointCount(4);
        float halfL = length / 2.f * expandScale;
        outer.setPoint(0, { -slashWidth * 2, -halfL });
        outer.setPoint(1, { slashWidth * 2, -halfL });
        outer.setPoint(2, { slashWidth * 2,  halfL });
        outer.setPoint(3, { -slashWidth * 2, halfL });
        outer.setFillColor(sf::Color(255, 80, 40, (sf::Uint8)(alpha * 0.5f)));
        outer.setPosition(center);
        outer.setRotation(dir > 0 ? 20.f : -20.f);
        w.draw(outer);

        sf::ConvexShape mid;
        mid.setPointCount(4);
        mid.setPoint(0, { -slashWidth, -halfL * 0.9f });
        mid.setPoint(1, { slashWidth, -halfL * 0.9f });
        mid.setPoint(2, { slashWidth,  halfL * 0.9f });
        mid.setPoint(3, { -slashWidth, halfL * 0.9f });
        mid.setFillColor(sf::Color(255, 150, 50, (sf::Uint8)alpha));
        mid.setPosition(center);
        mid.setRotation(dir > 0 ? 20.f : -20.f);
        w.draw(mid);

        sf::ConvexShape inner;
        inner.setPointCount(4);
        inner.setPoint(0, { -3.f, -halfL * 0.8f });
        inner.setPoint(1, { 3.f, -halfL * 0.8f });
        inner.setPoint(2, { 3.f,  halfL * 0.8f });
        inner.setPoint(3, { -3.f, halfL * 0.8f });
        inner.setFillColor(sf::Color(255, 240, 200, (sf::Uint8)alpha));
        inner.setPosition(center);
        inner.setRotation(dir > 0 ? 20.f : -20.f);
        w.draw(inner);

        sf::CircleShape wave(60.f + t * 60.f);
        wave.setPosition(center.x - (60.f + t * 60.f), center.y - (60.f + t * 60.f));
        wave.setFillColor(sf::Color::Transparent);
        wave.setOutlineColor(sf::Color(255, 80, 40, (sf::Uint8)(alpha * 0.4f)));
        wave.setOutlineThickness(3.f);
        w.draw(wave);
    }

    void drawBossDashTrail(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 200.f;

        sf::ConvexShape trail;
        trail.setPointCount(4);
        float length = 100.f + t * 40.f;
        float dir = e.facingRight ? -1.f : 1.f; 

        trail.setPoint(0, { 0.f, -20.f });
        trail.setPoint(1, { length * dir, -8.f });
        trail.setPoint(2, { length * dir, 8.f });
        trail.setPoint(3, { 0.f, 20.f });
        trail.setFillColor(sf::Color(255, 140, 30, (sf::Uint8)alpha));
        trail.setPosition(e.position);
        w.draw(trail);

        sf::ConvexShape innerTrail;
        innerTrail.setPointCount(4);
        innerTrail.setPoint(0, { 0.f, -10.f });
        innerTrail.setPoint(1, { length * dir * 0.8f, -4.f });
        innerTrail.setPoint(2, { length * dir * 0.8f, 4.f });
        innerTrail.setPoint(3, { 0.f, 10.f });
        innerTrail.setFillColor(sf::Color(255, 220, 100, (sf::Uint8)alpha));
        innerTrail.setPosition(e.position);
        w.draw(innerTrail);

        for (int i = 0; i < 3; ++i) {
            sf::CircleShape spark(3.f);
            spark.setPosition(
                e.position.x + dir * (20.f + i * 15.f),
                e.position.y + (std::rand() % 20 - 10)
            );
            spark.setFillColor(sf::Color(255, 200, 50, (sf::Uint8)(alpha * 0.7f)));
            w.draw(spark);
        }
    }

    void drawBossDangerSmash(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        sf::Vector2f center = e.position;

        float radius = 30.f + t * 200.f;

        sf::ConvexShape leftWave;
        leftWave.setPointCount(5);
        leftWave.setPoint(0, { 0.f, 0.f });
        leftWave.setPoint(1, { -radius, -radius * 0.6f });
        leftWave.setPoint(2, { -radius * 1.2f, 0.f });
        leftWave.setPoint(3, { -radius, radius * 0.6f });
        leftWave.setPoint(4, { 0.f, 0.f });
        leftWave.setFillColor(sf::Color(180, 20, 30, (sf::Uint8)(alpha * 0.5f)));
        leftWave.setOutlineColor(sf::Color(255, 50, 50, (sf::Uint8)alpha));
        leftWave.setOutlineThickness(3.f);
        leftWave.setPosition(center);
        w.draw(leftWave);

        sf::ConvexShape rightWave;
        rightWave.setPointCount(5);
        rightWave.setPoint(0, { 0.f, 0.f });
        rightWave.setPoint(1, { radius, -radius * 0.6f });
        rightWave.setPoint(2, { radius * 1.2f, 0.f });
        rightWave.setPoint(3, { radius, radius * 0.6f });
        rightWave.setPoint(4, { 0.f, 0.f });
        rightWave.setFillColor(sf::Color(180, 20, 30, (sf::Uint8)(alpha * 0.5f)));
        rightWave.setOutlineColor(sf::Color(255, 50, 50, (sf::Uint8)alpha));
        rightWave.setOutlineThickness(3.f);
        rightWave.setPosition(center);
        w.draw(rightWave);

        float pulse = std::sin(t * 3.14159f * 4) * 0.3f + 0.7f;
        sf::CircleShape core(30.f * pulse);
        core.setPosition(center.x - 30.f * pulse, center.y - 30.f * pulse);
        core.setFillColor(sf::Color(200, 30, 60, (sf::Uint8)alpha));
        w.draw(core);

        sf::CircleShape coreInner(20.f * pulse);
        coreInner.setPosition(center.x - 20.f * pulse, center.y - 20.f * pulse);
        coreInner.setFillColor(sf::Color(255, 150, 180, (sf::Uint8)alpha));
        w.draw(coreInner);

        for (int i = 0; i < 4; ++i) {
            float angle = i * 3.14159f / 2.f + t * 0.5f;
            float len = radius * 1.3f;
            sf::Vertex line[] = {
                sf::Vertex(center, sf::Color(255, 100, 100, (sf::Uint8)alpha)),
                sf::Vertex(
                    sf::Vector2f(center.x + std::cos(angle) * len,
                                 center.y + std::sin(angle) * len),
                    sf::Color(255, 50, 50, 0))
            };
            w.draw(line, 2, sf::Lines);
        }
    }

    void drawCrossSlash(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        sf::Vector2f center = e.position;
        float dir = e.facingRight ? 1.f : -1.f;

        float scale = 0.3f + t * 1.2f;
        float length = 200.f * scale;
        float thickness = 14.f * (1.f - t * 0.5f);

        drawSlashLine(w, center, length, thickness, -30.f,
            sf::Color(255, 80, 40, (sf::Uint8)alpha),
            sf::Color(255, 240, 200, (sf::Uint8)alpha));

        if (t > 0.2f) {
            float t2 = (t - 0.2f) / 0.8f;
            float alpha2 = (1.f - t2) * 255.f;
            drawSlashLine(w, center, length, thickness, 30.f,
                sf::Color(255, 80, 40, (sf::Uint8)alpha2),
                sf::Color(255, 240, 200, (sf::Uint8)alpha2));
        }

        float pulse = std::sin(t * 3.14159f * 6) * 0.3f + 0.7f;
        sf::CircleShape core(25.f * pulse * (1.f - t * 0.5f));
        core.setOrigin(core.getRadius(), core.getRadius());
        core.setPosition(center);
        core.setFillColor(sf::Color(255, 180, 80, (sf::Uint8)alpha));
        w.draw(core);

        sf::CircleShape inner(12.f * pulse * (1.f - t * 0.5f));
        inner.setOrigin(inner.getRadius(), inner.getRadius());
        inner.setPosition(center);
        inner.setFillColor(sf::Color(255, 255, 220, (sf::Uint8)alpha));
        w.draw(inner);

        for (int i = 0; i < 8; ++i) {
            float angle = i * 3.14159f / 4.f + t;
            float d = 60.f + t * 80.f;
            sf::CircleShape spark(3.f);
            spark.setPosition(
                center.x + std::cos(angle) * d - 3.f,
                center.y + std::sin(angle) * d - 3.f
            );
            spark.setFillColor(sf::Color(255, 200, 100, (sf::Uint8)(alpha * 0.7f)));
            w.draw(spark);
        }
    }

    void drawSlashLine(sf::RenderWindow& w, sf::Vector2f center,
        float length, float thickness, float angle,
        sf::Color outerColor, sf::Color innerColor) {
        sf::ConvexShape outer;
        outer.setPointCount(4);
        outer.setPoint(0, { -length / 2.f, -thickness });
        outer.setPoint(1, { length / 2.f, -thickness });
        outer.setPoint(2, { length / 2.f,  thickness });
        outer.setPoint(3, { -length / 2.f, thickness });
        outer.setFillColor(outerColor);
        outer.setPosition(center);
        outer.setRotation(angle);
        w.draw(outer);

        sf::ConvexShape inner;
        inner.setPointCount(4);
        inner.setPoint(0, { -length / 2.f * 0.9f, -thickness * 0.4f });
        inner.setPoint(1, { length / 2.f * 0.9f, -thickness * 0.4f });
        inner.setPoint(2, { length / 2.f * 0.9f,  thickness * 0.4f });
        inner.setPoint(3, { -length / 2.f * 0.9f, thickness * 0.4f });
        inner.setFillColor(innerColor);
        inner.setPosition(center);
        inner.setRotation(angle);
        w.draw(inner);
    }

    void drawIceSpike(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        sf::Vector2f target = e.position;   

        if (t < 0.5f) {
            float fallT = t / 0.5f;
            sf::Vector2f spikePos;
            spikePos.x = target.x;
            spikePos.y = target.y - (1.f - fallT) * 400.f;   

            sf::ConvexShape spike;
            spike.setPointCount(3);
            spike.setPoint(0, { 0, -60.f });     
            spike.setPoint(1, { -15.f, 20.f });  
            spike.setPoint(2, { 15.f, 20.f });   
            spike.setFillColor(sf::Color(200, 230, 255, 255));
            spike.setOutlineColor(sf::Color(150, 200, 255, 255));
            spike.setOutlineThickness(2.f);
            spike.setPosition(spikePos);
            w.draw(spike);

            sf::ConvexShape highlight;
            highlight.setPointCount(3);
            highlight.setPoint(0, { -3.f, -50.f });
            highlight.setPoint(1, { -8.f, 10.f });
            highlight.setPoint(2, { 2.f, 10.f });
            highlight.setFillColor(sf::Color(255, 255, 255, 200));
            highlight.setPosition(spikePos);
            w.draw(highlight);

            for (int i = 1; i <= 3; ++i) {
                sf::ConvexShape trail;
                trail.setPointCount(3);
                float tOffset = 20.f * i;
                trail.setPoint(0, { 0, -60.f + tOffset });
                trail.setPoint(1, { -10.f, 20.f + tOffset });
                trail.setPoint(2, { 10.f, 20.f + tOffset });
                trail.setFillColor(sf::Color(180, 220, 255, (sf::Uint8)(100 - i * 30)));
                trail.setPosition(spikePos);
                w.draw(trail);
            }
        }
        else {
            float impactT = (t - 0.5f) / 0.5f;
            float impactAlpha = (1.f - impactT) * 255.f;

            sf::CircleShape shockwave(20.f + impactT * 80.f);
            shockwave.setOrigin(shockwave.getRadius(), shockwave.getRadius());
            shockwave.setPosition(target);
            shockwave.setFillColor(sf::Color::Transparent);
            shockwave.setOutlineColor(sf::Color(150, 220, 255, (sf::Uint8)(impactAlpha * 0.8f)));
            shockwave.setOutlineThickness(3.f);
            w.draw(shockwave);

            sf::ConvexShape groundSpike;
            groundSpike.setPointCount(3);
            groundSpike.setPoint(0, { 0, -40.f });
            groundSpike.setPoint(1, { -12.f, 15.f });
            groundSpike.setPoint(2, { 12.f, 15.f });
            groundSpike.setFillColor(sf::Color(200, 230, 255, (sf::Uint8)impactAlpha));
            groundSpike.setOutlineColor(sf::Color(100, 180, 255, (sf::Uint8)impactAlpha));
            groundSpike.setOutlineThickness(2.f);
            groundSpike.setPosition(target);
            w.draw(groundSpike);

            for (int i = 0; i < 8; ++i) {
                float angle = i * 3.14159f / 4.f;
                float d = impactT * 60.f;
                sf::CircleShape shard(3.f + i % 2);
                shard.setPosition(
                    target.x + std::cos(angle) * d - 3.f,
                    target.y + std::sin(angle) * d - 3.f
                );
                shard.setFillColor(sf::Color(180, 220, 255, (sf::Uint8)impactAlpha));
                w.draw(shard);
            }
        }
    }
    void drawBossSweep(sf::RenderWindow& w, const Effect& e, float t) {
        float alpha = (1.f - t) * 255.f;
        sf::Vector2f center = e.position;

        float dir = e.facingRight ? 1.f : -1.f;
        float sweepRadius = 60.f + t * 180.f;

        for (int i = 0; i < 12; ++i) {
            float angle1 = (-60.f + i * 10.f) * 3.14159f / 180.f;
            float angle2 = (-60.f + (i + 1) * 10.f) * 3.14159f / 180.f;

            sf::ConvexShape slice;
            slice.setPointCount(3);
            slice.setPoint(0, { 0.f, 0.f });
            slice.setPoint(1, { std::cos(angle1) * sweepRadius * dir, std::sin(angle1) * sweepRadius });
            slice.setPoint(2, { std::cos(angle2) * sweepRadius * dir, std::sin(angle2) * sweepRadius });

            sf::Uint8 sliceAlpha = (sf::Uint8)(alpha * (0.4f + 0.5f * (i / 12.f)));
            slice.setFillColor(sf::Color(200, 20, 40, sliceAlpha));
            slice.setPosition(center);
            w.draw(slice);
        }

        float innerR = sweepRadius * 0.7f;
        sf::ConvexShape innerArc;
        innerArc.setPointCount(13);
        innerArc.setPoint(0, { 0, 0 });
        for (int i = 0; i < 12; ++i) {
            float a = (-60.f + i * 10.f) * 3.14159f / 180.f;
            innerArc.setPoint(i + 1, { std::cos(a) * innerR * dir, std::sin(a) * innerR });
        }
        innerArc.setFillColor(sf::Color(255, 80, 40, (sf::Uint8)(alpha * 0.6f)));
        innerArc.setPosition(center);
        w.draw(innerArc);

        sf::CircleShape rim(sweepRadius);
        rim.setOrigin(sweepRadius, sweepRadius);
        rim.setPosition(center);
        rim.setFillColor(sf::Color::Transparent);
        rim.setOutlineColor(sf::Color(255, 100, 60, (sf::Uint8)alpha));
        rim.setOutlineThickness(4.f);
        w.draw(rim);

        for (int i = 0; i < 12; ++i) {
            float angle = (-60.f + i * 10.f) * 3.14159f / 180.f;
            float d = sweepRadius * (0.8f + t * 0.3f);
            sf::CircleShape spark(4.f + (std::rand() % 3));
            spark.setPosition(
                center.x + std::cos(angle) * d * dir - 4.f,
                center.y + std::sin(angle) * d - 4.f
            );
            spark.setFillColor(sf::Color(255, 180, 60, (sf::Uint8)alpha));
            w.draw(spark);
        }

        float pulse = std::sin(t * 3.14159f * 8) * 0.3f + 0.7f;
        sf::CircleShape core(20.f * pulse * (1.f - t * 0.5f));
        core.setOrigin(core.getRadius(), core.getRadius());
        core.setPosition(center);
        core.setFillColor(sf::Color(255, 200, 100, (sf::Uint8)alpha));
        w.draw(core);
    }
    EffectSystem() = default;
};
