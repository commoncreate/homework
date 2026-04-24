#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.h"
#include "DummyEnemy.h"
#include "EnemyPlaceholder.h"
#include "TileMap.h"
#include "Camera.h"
#include "LevelManager.h"

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
    void loadLevel1();

    sf::RenderWindow window;
    Player player;
    std::unique_ptr<DummyEnemy> dummy;
    std::vector<std::unique_ptr<EnemyPlaceholder>> enemies;

    TileMap map;
    Camera camera;
    LevelManager levelMgr;

    sf::View uiView;
    sf::RectangleShape parryFlash;
    sf::RectangleShape fadeOverlay;
    sf::RectangleShape skyBg;         
};