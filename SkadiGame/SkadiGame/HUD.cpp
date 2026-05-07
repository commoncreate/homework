#include "HUD.h"
#include "Player.h"
#include "Boss.h"

void HUD::render(sf::RenderWindow& w, const Player& player, const Boss* boss) {
    sf::RectangleShape hpBg({ 300.f, 20.f });
    hpBg.setPosition(20.f, 20.f);
    hpBg.setFillColor(sf::Color(40, 0, 0, 200));
    hpBg.setOutlineColor(sf::Color::White);
    hpBg.setOutlineThickness(2.f);
    w.draw(hpBg);

    float hpP = (float)player.getHP() / 100.f;
    sf::RectangleShape hpFg({ 300.f * hpP, 20.f });
    hpFg.setPosition(20.f, 20.f);
    hpFg.setFillColor(sf::Color(220, 50, 50));
    w.draw(hpFg);

    sf::RectangleShape pstBg({ 300.f, 10.f });
    pstBg.setPosition(20.f, 48.f);
    pstBg.setFillColor(sf::Color(30, 30, 30, 200));
    pstBg.setOutlineColor(sf::Color::White);
    pstBg.setOutlineThickness(1.f);
    w.draw(pstBg);

    float pp = player.getPosture() / 100.f;
    sf::RectangleShape pstFg({ 300.f * pp, 10.f });
    pstFg.setPosition(20.f, 48.f);
    if (pp > 0.8f) pstFg.setFillColor(sf::Color(255, 60, 60));
    else if (pp > 0.5f) pstFg.setFillColor(sf::Color(255, 150, 50));
    else pstFg.setFillColor(sf::Color(255, 220, 50));
    w.draw(pstFg);

    if (boss && !boss->isDead()) {
        sf::RectangleShape bBg({ 800.f, 25.f });
        bBg.setPosition(240.f, 660.f);
        bBg.setFillColor(sf::Color(40, 0, 0, 220));
        bBg.setOutlineColor(sf::Color(255, 100, 100));
        bBg.setOutlineThickness(2.f);
        w.draw(bBg);

        float bhp = (float)boss->getHP() / boss->getMaxHP();
        sf::RectangleShape bFg({ 800.f * bhp, 25.f });
        bFg.setPosition(240.f, 660.f);
        bFg.setFillColor(sf::Color(180, 40, 40));
        w.draw(bFg);

        sf::RectangleShape bPstBg({ 800.f, 12.f });
        bPstBg.setPosition(240.f, 690.f);
        bPstBg.setFillColor(sf::Color(30, 30, 30, 200));
        w.draw(bPstBg);

        float bpp = boss->getPosturePercent();
        sf::RectangleShape bPstFg({ 800.f * bpp, 12.f });
        bPstFg.setPosition(240.f, 690.f);
        if (bpp > 0.8f) bPstFg.setFillColor(sf::Color(255, 60, 60));
        else if (bpp > 0.5f) bPstFg.setFillColor(sf::Color(255, 150, 50));
        else bPstFg.setFillColor(sf::Color(255, 220, 50));
        w.draw(bPstFg);

        for (int i = 1; i <= 2; ++i) {
            sf::RectangleShape mark({ 2.f, 25.f });
            mark.setPosition(240.f + 800.f * (i / 3.f), 660.f);
            mark.setFillColor(sf::Color::White);
            w.draw(mark);
        }
    }

    if (boss && boss->canBeExecuted()) {
        sf::RectangleShape warn({ 400.f, 40.f });
        warn.setPosition(440.f, 300.f);
        warn.setFillColor(sf::Color(255, 50, 50, 180));
        w.draw(warn);
    }
}

void HUD::renderVictory(sf::RenderWindow& w) {
    sf::RectangleShape bg({ 1280.f, 720.f });
    bg.setFillColor(sf::Color(0, 0, 0, 200));
    w.draw(bg);

    sf::RectangleShape panel({ 600.f, 200.f });
    panel.setPosition(340.f, 260.f);
    panel.setFillColor(sf::Color(30, 60, 100));
    panel.setOutlineColor(sf::Color(100, 200, 255));
    panel.setOutlineThickness(4.f);
    w.draw(panel);
}

void HUD::renderDefeat(sf::RenderWindow& w) {
    sf::RectangleShape bg({ 1280.f, 720.f });
    bg.setFillColor(sf::Color(40, 0, 0, 200));
    w.draw(bg);
}
