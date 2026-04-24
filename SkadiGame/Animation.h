#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    Animation() = default;

    void setup(const sf::Texture& texture, int frameCount,
        int frameW, int frameH, float frameDuration,
        bool loop = true);

    void update(float dt);
    void reset();                         
    bool isFinished() const { return finished; }
    int  getCurrentFrame() const { return currentFrame; }

    void applyTo(sf::Sprite& sprite) const;

private:
    const sf::Texture* tex = nullptr;
    int   frameCount = 1;
    int   frameW = 0, frameH = 0;
    float frameDuration = 0.1f;
    bool  loop = true;
    int   currentFrame = 0;
    float timer = 0.f;
    bool  finished = false;
};
