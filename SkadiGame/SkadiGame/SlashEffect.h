#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

class SlashEffect {
public:
    void trigger(sf::Vector2f pos, bool facingRight) {
        active = true;
        timer = DURATION;
        position = pos;
        this->facingRight = facingRight;
    }

    void update(float dt) {
        if (active) {
            timer -= dt;
            if (timer <= 0.f) active = false;
        }
    }

    void render(sf::RenderWindow& w) {
        if (!active) return;

        float t = 1.f - timer / DURATION;   
        float dir = facingRight ? 1.f : -1.f;

        sf::VertexArray arc(sf::TriangleStrip);

        const int SEGMENTS = 20;
        float reach = 140.f;
        float angleStart = -70.f * 3.14159f / 180.f;    
        float angleEnd = 70.f * 3.14159f / 180.f;    

        float animProgress = std::min(1.f, t * 1.5f);    
        float currentEnd = angleStart + (angleEnd - angleStart) * animProgress;

        for (int i = 0; i <= SEGMENTS; ++i) {
            float a = angleStart + (currentEnd - angleStart) * (float)i / SEGMENTS;
            float sinA = std::sin(a);
            float cosA = std::cos(a);

            sf::Vector2f outer(
                position.x + cosA * reach * dir,
                position.y + sinA * reach
            );
            sf::Vector2f inner(
                position.x + cosA * (reach - 25.f) * dir,
                position.y + sinA * (reach - 25.f)
            );

            sf::Uint8 alpha = (sf::Uint8)((1.f - t) * 240);

            float freshness = (float)i / SEGMENTS;
            sf::Uint8 brightAlpha = (sf::Uint8)(alpha * (0.3f + 0.7f * freshness));

            arc.append(sf::Vertex(outer, sf::Color(180, 230, 255, brightAlpha)));
            arc.append(sf::Vertex(inner, sf::Color(255, 255, 255, brightAlpha)));
        }
        w.draw(arc);

        float coreA = angleStart + (currentEnd - angleStart) * 0.95f;
        sf::Vector2f coreHead(
            position.x + std::cos(coreA) * reach * dir,
            position.y + std::sin(coreA) * reach
        );

        sf::CircleShape flash(8.f * (1.f - t * 0.5f));
        flash.setPosition(coreHead.x - flash.getRadius(), coreHead.y - flash.getRadius());
        flash.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)((1.f - t) * 230)));
        w.draw(flash);

        for (int i = 0; i < 6; ++i) {
            float pA = angleStart + (currentEnd - angleStart) * (0.3f + i * 0.1f);
            float pReach = reach + (float)(i % 3 - 1) * 8.f;
            sf::Vector2f ppos(
                position.x + std::cos(pA) * pReach * dir,
                position.y + std::sin(pA) * pReach
            );

            sf::CircleShape p(2.f + i * 0.3f);
            p.setPosition(ppos.x - p.getRadius(), ppos.y - p.getRadius());
            p.setFillColor(sf::Color(200, 240, 255, (sf::Uint8)((1.f - t) * 180)));
            w.draw(p);
        }
    }

    bool isActive() const { return active; }

private:
    bool active = false;
    sf::Vector2f position;
    bool facingRight = true;
    float timer = 0.f;
    const float DURATION = 0.30f;
};