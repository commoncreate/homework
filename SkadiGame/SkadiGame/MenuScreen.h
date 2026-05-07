#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class MenuScreen {
public:
    enum class Type { START, VICTORY, DEFEAT };

    MenuScreen(Type t = Type::START);

    void loadFont();
    void update(float dt);
    bool handleInput();
    void render(sf::RenderWindow& w);

    void setType(Type t);
    Type getType() const { return type; }

private:
    Type type;
    float time = 0.f;

    sf::Font font;
    bool hasFont = false;
    bool enterPrev = false;
    bool rPrev = false;

    void drawPixelText(sf::RenderWindow& w, const std::string& text,
        float x, float y, float size, sf::Color color);
};