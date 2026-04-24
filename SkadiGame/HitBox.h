#pragma once
#include <SFML/Graphics.hpp>

struct HitBox {
    sf::FloatRect rect;
    int damage = 10;
    float postureDamage = 10.f;
    bool active = false;          
    float lifeTime = 0.f;         
    int ownerId = 0;              

    void update(float dt) {
        if (active) {
            lifeTime -= dt;
            if (lifeTime <= 0.f) active = false;
        }
    }
};
