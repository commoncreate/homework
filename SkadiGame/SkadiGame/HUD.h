#pragma once
#include <SFML/Graphics.hpp>

class Player;
class Boss;

class HUD {
public:
    void render(sf::RenderWindow& w, const Player& player, const Boss* boss);
    void renderVictory(sf::RenderWindow& w);
    void renderDefeat(sf::RenderWindow& w);
};
