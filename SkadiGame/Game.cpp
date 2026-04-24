#include "Game.h"
#include <iostream>
#include <filesystem>

Game::Game()
    : window(sf::VideoMode(1280, 720), "Skadi - Abyssal Hunt v1.0"),
    camera({ 1280.f, 720.f })
{
    window.setFramerateLimit(60);

    std::cout << "[DEBUG] cwd: "
        << std::filesystem::current_path().string() << "\n";

    player.loadAssets();
    loadLevel1();

    dummy = std::make_unique<DummyEnemy>(sf::Vector2f(600.f, 35.f * 16.f));

    uiView = window.getDefaultView();
    parryFlash.setSize({ 1280.f, 720.f });
    parryFlash.setFillColor(sf::Color::Transparent);
    fadeOverlay.setSize({ 1280.f, 720.f });
    fadeOverlay.setFillColor(sf::Color::Transparent);

    skyBg.setSize({ 1280.f, 720.f });
    skyBg.setFillColor(sf::Color(120, 160, 180));
}

void Game::loadLevel1() {
    bool ok = map.generateArenaLevel("assets/tilesets/tileset.png", 16);
    if (!ok) {
        std::cerr << "[FATAL] 场地生成失败\n";
        return;
    }

    camera.setHorizontalMode(map.getHeightPx() / 2.f);

    enemies.clear();

    float groundY = 17.f * 16.f;       
    player.setPosition({ 100.f, groundY });
    dummy = std::make_unique<DummyEnemy>(
        sf::Vector2f(map.getWidthPx() - 200.f, map.getGroundY())
    );


    std::cout << "[OK] Arena loaded. GroundY=" << groundY << "\n";
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        handleInput();
        update(dt);
        render();
    }
}

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape) window.close();
    }
    if (!levelMgr.isTransitioning()) player.handleInput(0.f);
}

void Game::update(float dt) {
    levelMgr.update(dt);
    if (levelMgr.isTransitioning()) return;

    player.update(dt);
    resolveTileCollisionX();
    resolveTileCollisionY();

    for (auto& e : enemies) e->update(dt);

    sf::FloatRect pb = player.getBounds();
    dummy->update(dt, { pb.left + pb.width / 2.f, pb.top });
    HitBox& atk = dummy->getHitBox();
    if (atk.active && atk.rect.intersects(pb)) {
        player.onHit(atk);
        atk.active = false;
    }

    camera.follow(player.getPosition(), dt, 6.f);
    camera.clampToBounds(map.getWidthPx(), map.getHeightPx());
    camera.updateShake(dt);
}

void Game::resolveTileCollisionX() {
    sf::FloatRect b = player.getBounds();
    unsigned ts = map.getTileSize();
    float topY = b.top + 4.f;
    float midY = b.top + b.height / 2.f;
    float botY = b.top + b.height - 4.f;

    float rX = b.left + b.width + 1.f;
    if (map.isSolidAt(rX, topY) || map.isSolidAt(rX, midY) || map.isSolidAt(rX, botY)) {
        int col = (int)(rX / ts);
        float wallX = (float)col * ts;
        player.pushLeftTo(wallX - b.width / 2.f - 1.f);
    }
    float lX = b.left - 1.f;
    if (map.isSolidAt(lX, topY) || map.isSolidAt(lX, midY) || map.isSolidAt(lX, botY)) {
        int col = (int)(lX / ts);
        float wallX = (float)(col + 1) * ts;
        player.pushRightTo(wallX + b.width / 2.f + 1.f);
    }
}

void Game::resolveTileCollisionY() {
    sf::FloatRect b = player.getBounds();
    unsigned ts = map.getTileSize();
    float lX = b.left + 4.f;
    float mX = b.left + b.width / 2.f;
    float rX = b.left + b.width - 4.f;

    float feetY = b.top + b.height + 1.f;
    if (map.isSolidAt(lX, feetY) || map.isSolidAt(mX, feetY) || map.isSolidAt(rX, feetY)) {
        int row = (int)(feetY / ts);
        float topY = (float)row * ts;
        player.landOnGround(topY);
    }
    float headY = b.top - 1.f;
    if (map.isSolidAt(lX, headY) || map.isSolidAt(mX, headY) || map.isSolidAt(rX, headY)) {
        int row = (int)(headY / ts);
        float botOfTile = (float)(row + 1) * ts;
        player.hitCeiling(botOfTile);
    }
}

void Game::render() {
    window.setView(uiView);
    window.draw(skyBg);

    window.setView(camera.get());
    window.draw(map);
    for (auto& e : enemies) e->render(window);
    dummy->render(window);
    player.render(window);

    window.setView(uiView);

    float a = player.getParryFlashAlpha();
    if (a > 0.f) {
        parryFlash.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)(a * 180)));
        window.draw(parryFlash);
    }

    float fa = levelMgr.getFadeAlpha();
    if (fa > 0.f) {
        fadeOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(fa * 255)));
        window.draw(fadeOverlay);
    }

    window.display();
}