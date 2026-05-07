#include "MenuScreen.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>

MenuScreen::MenuScreen(Type t) : type(t) {}

void MenuScreen::loadFont() {
    if (font.loadFromFile("C:/Windows/Fonts/msyh.ttc")) {
        hasFont = true;
        std::cout << "[OK] Font loaded (msyh)\n";
        return;
    }
    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf")) {
        hasFont = true;
        std::cout << "[OK] Font loaded (simhei)\n";
        return;
    }
    if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        hasFont = true;
        std::cout << "[OK] Font loaded (arial)\n";
        return;
    }
    if (font.loadFromFile("assets/fonts/zh.ttf")) {
        hasFont = true;
        std::cout << "[OK] Font loaded (assets)\n";
        return;
    }
    hasFont = false;
    std::cout << "[WARN] No font found, using pixel text\n";
}

void MenuScreen::update(float dt) { time += dt; }

bool MenuScreen::handleInput() {
    bool enterNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
    bool rNow = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

    bool triggered = false;

   
   if (type == Type::START && enterNow && !enterPrev) triggered = true;
   if ((type == Type::VICTORY || type == Type::DEFEAT) && rNow && !rPrev) triggered = true;
   

    enterPrev = enterNow;
    rPrev = rNow;
    return triggered;
}

void MenuScreen::setType(Type t) { 
    type = t; 
    time = 0.f; 
    enterPrev = false;   
    rPrev = false;
}

void MenuScreen::render(sf::RenderWindow& w) {
    sf::RectangleShape bg({ 1280.f, 720.f });
    if (type == Type::START)        bg.setFillColor(sf::Color(10, 20, 40, 240));
    else if (type == Type::VICTORY) bg.setFillColor(sf::Color(20, 40, 80, 220));
    else                            bg.setFillColor(sf::Color(60, 0, 0, 220));
    w.draw(bg);

    sf::RectangleShape panel({ 800.f, 300.f });
    panel.setPosition(240.f, 210.f);
    panel.setFillColor(sf::Color(30, 50, 80, 240));
    panel.setOutlineColor(sf::Color(100, 200, 255));
    panel.setOutlineThickness(4.f);
    w.draw(panel);

    float pulse = 0.7f + 0.3f * std::sin(time * 2.f);
    sf::Color tc;
    if (type == Type::START)        tc = sf::Color(100, 200, 255);
    else if (type == Type::VICTORY) tc = sf::Color(255, 220, 100);
    else                            tc = sf::Color(220, 80, 80);
    tc.a = (sf::Uint8)(pulse * 255);

    if (hasFont) {
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(72);
        title.setFillColor(tc);
        title.setStyle(sf::Text::Bold);

        std::string ts;
        if (type == Type::START)        ts = "SKADI: ABYSSAL HUNT";
        else if (type == Type::VICTORY) ts = "VICTORY!";
        else                            ts = "YOU DIED";
        title.setString(ts);

        sf::FloatRect b = title.getLocalBounds();
        title.setOrigin(b.width / 2.f, b.height / 2.f);
        title.setPosition(640.f, 300.f);
        w.draw(title);

        sf::Text sub;
        sub.setFont(font);
        sub.setCharacterSize(28);
        sub.setFillColor(sf::Color::White);
        if (type == Type::START)
            sub.setString("Press ENTER to Start");
        else
            sub.setString("Press R to Restart  |  ESC to Quit");
        sf::FloatRect sb = sub.getLocalBounds();
        sub.setOrigin(sb.width / 2.f, sb.height / 2.f);
        sub.setPosition(640.f, 450.f);

        int blink = (int)(time * 2.f) % 2;
        if (blink) w.draw(sub);

    }
    else {
        std::string ts;
        if (type == Type::START)        ts = "SKADI";
        else if (type == Type::VICTORY) ts = "VICTORY";
        else                            ts = "YOU DIED";
        drawPixelText(w, ts, 640.f, 300.f, 12.f, tc);

        int blink = (int)(time * 2.f) % 2;
        if (blink) {
            std::string sub = (type == Type::START) ? "PRESS ENTER" : "PRESS R";
            drawPixelText(w, sub, 640.f, 450.f, 6.f, sf::Color::White);
        }
    }
}

void MenuScreen::drawPixelText(sf::RenderWindow& w, const std::string& text,
    float cx, float cy, float size, sf::Color color) {
    static const std::unordered_map<char, std::vector<std::string>> font5x7 = {
        {'A', {"01110","10001","10001","11111","10001","10001","10001"}},
        {'B', {"11110","10001","10001","11110","10001","10001","11110"}},
        {'C', {"01110","10001","10000","10000","10000","10001","01110"}},
        {'D', {"11110","10001","10001","10001","10001","10001","11110"}},
        {'E', {"11111","10000","10000","11110","10000","10000","11111"}},
        {'G', {"01110","10001","10000","10111","10001","10001","01110"}},
        {'H', {"10001","10001","10001","11111","10001","10001","10001"}},
        {'I', {"11111","00100","00100","00100","00100","00100","11111"}},
        {'K', {"10001","10010","10100","11000","10100","10010","10001"}},
        {'N', {"10001","11001","10101","10101","10101","10011","10001"}},
        {'O', {"01110","10001","10001","10001","10001","10001","01110"}},
        {'P', {"11110","10001","10001","11110","10000","10000","10000"}},
        {'R', {"11110","10001","10001","11110","10100","10010","10001"}},
        {'S', {"01111","10000","10000","01110","00001","00001","11110"}},
        {'T', {"11111","00100","00100","00100","00100","00100","00100"}},
        {'U', {"10001","10001","10001","10001","10001","10001","01110"}},
        {'V', {"10001","10001","10001","10001","10001","01010","00100"}},
        {'Y', {"10001","10001","01010","00100","00100","00100","00100"}},
        {' ', {"00000","00000","00000","00000","00000","00000","00000"}},
        {'!', {"00100","00100","00100","00100","00100","00000","00100"}},
    };

    float charW = 5 * size;
    float charH = 7 * size;
    float spacing = size;
    float totalW = text.size() * (charW + spacing);
    float startX = cx - totalW / 2.f;
    float startY = cy - charH / 2.f;

    for (size_t ci = 0; ci < text.size(); ++ci) {
        char c = (char)std::toupper((unsigned char)text[ci]);
        auto it = font5x7.find(c);
        if (it == font5x7.end()) continue;
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (it->second[row][col] == '1') {
                    sf::RectangleShape px({ size, size });
                    px.setPosition(
                        startX + ci * (charW + spacing) + col * size,
                        startY + row * size);
                    px.setFillColor(color);
                    w.draw(px);
                }
            }
        }
    }
}