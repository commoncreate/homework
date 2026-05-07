#include "Game.h"
#include "AssetManager.h"
#include "HitStop.h"
#include <iostream>
#include "AudioManager.h"
#include "EffectSystem.h"
#include "Projectile.h"

Game::Game()
    : window(sf::VideoMode(1280, 720), "Skadi v2.0 - Boss Fight"),
    camera({ 1280.f, 720.f })
{
    window.setFramerateLimit(60);
    player.loadAssets();

    uiView = window.getDefaultView();
    parryFlash.setSize({ 1280.f, 720.f });
    parryFlash.setFillColor(sf::Color::Transparent);

    skyBg.setSize({ 1280.f, 720.f });
    skyBg.setFillColor(sf::Color(120, 160, 180));   
    menu.loadFont();


    auto& A = AudioManager::instance();
    std::cout << "=== 加载音频资源 ===\n";
    A.loadSound("slash", "assets/audio/sfx_slash.wav");
    A.loadSound("hit", "assets/audio/sfx_hit.wav");
    A.loadSound("parry", "assets/audio/sfx_parry.wav");
    A.loadSound("block", "assets/audio/sfx_block.wav");
    A.loadSound("danger", "assets/audio/sfx_danger.wav");
    A.loadSound("execute", "assets/audio/sfx_execute.wav");
    A.loadSound("hurt", "assets/audio/sfx_hurt.wav");
    std::cout << "=== 音频加载完成 ===\n";
    
}

void Game::loadArena() {
    player.reset();
    bool ok = map.generateArenaLevel("assets/tilesets/tileset.png", 16);
    if (!ok) { std::cerr << "[FATAL] arena failed\n"; return; }

    float groundY = map.getGroundY();
    camera.setHorizontalMode(map.getHeightPx() / 2.f);

    player.setPosition({ 150.f, groundY });

    boss = std::make_unique<Boss>(sf::Vector2f(map.getWidthPx() - 250.f, groundY));
    boss->loadAssets();   
    EffectSystem::instance().clear();
    auto& AM = AssetManager::instance();
    sf::Texture tmpCheck;
    if (tmpCheck.loadFromFile("assets/bg/reeds_bg.png")) {
        const sf::Texture& bgTex = AM.loadTexture("bg_reeds", "assets/bg/reeds_bg.png");
        bgSprite.setTexture(bgTex);
        sf::Vector2u ts = bgTex.getSize();
        bgSprite.setScale(1280.f / ts.x, 720.f / ts.y);
        hasBgTexture = true;
        std::cout << "[OK] Background loaded\n";
    }
    else {
        hasBgTexture = false;
        std::cout << "[WARN] No background image, using sky color\n";
    }
    AudioManager::instance().playBGM("assets/audio/bgm_boss.ogg", 40.f);
    std::cout << "[OK] Arena loaded. GroundY=" << groundY << "\n";
    
    menu.loadFont();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float realDt = clock.restart().asSeconds();
        if (realDt > 0.05f) realDt = 0.05f;

        HitStop::instance().tick(realDt);

        handleInput();

        float gameDt = HitStop::instance().isFrozen() ? 0.f : realDt;
        update(gameDt);
        render();
    }
}

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape) {
            if (gameState == GameState::PLAYING) {
                gameState = GameState::MENU;
                menu.setType(MenuScreen::Type::START);
            } else {
                window.close();
            }
        }
    }
    if (gameState == GameState::PLAYING) player.handleInput(0.f);
}

void Game::update(float dt) {
    static int superDebug = 0;
    if (++superDebug % 60 == 0) {
        const char* sn =
            (gameState == GameState::MENU) ? "MENU" :
            (gameState == GameState::PLAYING) ? "PLAYING" :
            (gameState == GameState::VICTORY) ? "VICTORY" : "DEFEAT";
        std::cout << "[STATE] " << sn
            << " | Player HP=" << player.getHP()
            << " | Boss HP=" << (boss ? boss->getHP() : -1)
            << " | R=" << sf::Keyboard::isKeyPressed(sf::Keyboard::R)
            << " | Enter=" << sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)
            << "\n";
    }

    if (gameState == GameState::MENU ||
        gameState == GameState::VICTORY ||
        gameState == GameState::DEFEAT) {
        menu.update(dt);
        if (menu.handleInput()) {
            std::cout << "[TRIGGER] Loading arena!\n";
            loadArena();
            gameState = GameState::PLAYING;
        }
        return;
    }
    EffectSystem::instance().update(dt);
    player.update(dt);
    resolveTileCollisionX();
    resolveTileCollisionY();

    if (boss) {
        sf::Vector2f pp = player.getPosition();
        boss->update(dt, pp, map.getArenaLeft(), map.getArenaRight());
        HitBox& pAtk = player.getAttack().getHitBox();
        if (pAtk.active && pAtk.rect.intersects(boss->getBounds())) {
            boss->takeHit(pAtk, false);
            pAtk.active = false;
            HitStop::instance().freeze(0.08f);
            camera.shake(0.2f, 5.f);
        }
        for (auto& proj : player.getProjectiles()) {
            HitBox& ph = proj.getHitBox();
            if (ph.active && ph.rect.intersects(boss->getBounds())) {
                boss->takeHit(ph, true);
                proj.kill();
                HitStop::instance().freeze(0.05f);
                camera.shake(0.15f, 4.f);
            }
        }
        HitBox& yHit = player.getSkillHitbox_Y();
        if (yHit.active && yHit.rect.intersects(boss->getBounds())) {
            boss->takeHit(yHit, true);
            yHit.active = false;   
            HitStop::instance().freeze(0.15f);
            camera.shake(0.4f, 8.f);
            std::cout << "[HIT] Cross Slash hit Boss!\n";
        }
        HitBox& uHit = player.getSkillHitbox_U();
        if (uHit.active && uHit.rect.intersects(boss->getBounds())) {
            boss->takeHit(uHit);
            uHit.active = false;
            HitStop::instance().freeze(0.20f);
            camera.shake(0.5f, 10.f);
            std::cout << "[HIT] Ice Spike hit Boss!\n";
        }

        HitBox& bAtk = boss->getAttackHitBox();
        if (bAtk.active && bAtk.rect.intersects(player.getBounds())) {
            player.onHit(bAtk);
            bAtk.active = false;
        }

        for (auto& bp : boss->getCounterProjectiles()) {
            HitBox& bh = bp.getHitBox();
            if (bh.active && bh.rect.intersects(player.getBounds())) {
                player.onHit(bh);
                bp.kill();
                camera.shake(0.3f, 7.f);
            }
        }


        player.tryExecute(*boss);


        player.tryExecute(*boss);

        if (boss->isDead() && gameState == GameState::PLAYING) {
            gameState = GameState::VICTORY;
            menu.setType(MenuScreen::Type::VICTORY);
            AudioManager::instance().stopBGM();         
            AudioManager::instance().playSound("execute", 80.f);  
        }
        if (player.isDead() && gameState == GameState::PLAYING) {
            gameState = GameState::DEFEAT;
            menu.setType(MenuScreen::Type::DEFEAT);
            AudioManager::instance().stopBGM();        
            AudioManager::instance().playSound("hurt", 100.f);    
        }
    }

    camera.follow(player.getPosition(), dt, 6.f);
    camera.clampToBounds(map.getWidthPx(), map.getHeightPx());
    camera.updateShake(dt);
}

void Game::resolveTileCollisionX() {
    sf::FloatRect b = player.getBounds();
    unsigned ts = map.getTileSize();
    float tY = b.top + 4.f, mY = b.top + b.height / 2.f, bY = b.top + b.height - 4.f;

    float rX = b.left + b.width + 1.f;
    if (map.isSolidAt(rX, tY) || map.isSolidAt(rX, mY) || map.isSolidAt(rX, bY)) {
        int col = (int)(rX / ts);
        player.pushLeftTo((float)col * ts - b.width / 2.f - 1.f);
    }
    float lX = b.left - 1.f;
    if (map.isSolidAt(lX, tY) || map.isSolidAt(lX, mY) || map.isSolidAt(lX, bY)) {
        int col = (int)(lX / ts);
        player.pushRightTo((float)(col + 1) * ts + b.width / 2.f + 1.f);
    }
}

void Game::resolveTileCollisionY() {
    sf::FloatRect b = player.getBounds();
    unsigned ts = map.getTileSize();
    float lX = b.left + 4.f, mX = b.left + b.width / 2.f, rX = b.left + b.width - 4.f;

    float fY = b.top + b.height + 1.f;
    if (map.isSolidAt(lX, fY) || map.isSolidAt(mX, fY) || map.isSolidAt(rX, fY)) {
        int row = (int)(fY / ts);
        player.landOnGround((float)row * ts);
    }
    float hY = b.top - 1.f;
    if (map.isSolidAt(lX, hY) || map.isSolidAt(mX, hY) || map.isSolidAt(rX, hY)) {
        int row = (int)(hY / ts);
        player.hitCeiling((float)(row + 1) * ts);
    }
}

void Game::render() {
    window.setView(uiView);
    window.draw(skyBg);
    if (hasBgTexture) window.draw(bgSprite);

    window.setView(camera.get());
    window.draw(map);
    if (boss) boss->render(window);
    player.render(window);
    EffectSystem::instance().render(window);   

    float a = player.getParryFlashAlpha();
    if (a > 0.f) {
        parryFlash.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)(a * 180)));
        window.draw(parryFlash);
    }

    if (boss && gameState == GameState::PLAYING) {
        boss->renderUI(window, camera.getCenter());
    }

    if (gameState == GameState::PLAYING) {
        hud.render(window, player, boss.get());
    }
    else {
        menu.render(window);
    }

    /*window.setView(camera.get());
    window.draw(map);
    if (boss) boss->render(window);
    player.render(window);
    EffectSystem::instance().render(window);
    window.setView(uiView);*/

    window.display();
}