#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Player.h"
#include "Boss.h"
#include "TileMap.h"
#include "Camera.h"
#include "HUD.h"
#include "MenuScreen.h"

class Game {
public:
    Game();
    void run();

private:
    void handleInput();
    void update(float dt);
    void render();

    void resolveTileCollisionX();
    void resolveTileCollisionY();
    void loadArena();

    sf::RenderWindow window;
    Player player;
    std::unique_ptr<Boss> boss;

    TileMap map;
    Camera camera;
    HUD hud;

    sf::View uiView;
    sf::RectangleShape parryFlash;
    sf::RectangleShape skyBg;

    sf::Sprite bgSprite;
    bool hasBgTexture = false;

    enum class GameState { MENU, PLAYING, VICTORY, DEFEAT };
    GameState gameState = GameState::MENU;
    MenuScreen menu{ MenuScreen::Type::START };

};