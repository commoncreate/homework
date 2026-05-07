#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class LevelID {
    LEVEL_1_REEDS,      
    LEVEL_2_ABYSS,     
    NONE
};

class LevelManager {
public:
    LevelID currentLevel = LevelID::LEVEL_1_REEDS;

    enum class TransitionState { NONE, FADE_OUT, LOADING, FADE_IN };
    TransitionState transState = TransitionState::NONE;
    float transTimer = 0.f;
    LevelID pendingLevel = LevelID::NONE;

    void requestTransition(LevelID target) {
        if (transState != TransitionState::NONE) return;
        transState = TransitionState::FADE_OUT;
        transTimer = 1.0f;      
        pendingLevel = target;
    }

    void update(float dt) {
        if (transState == TransitionState::NONE) return;
        transTimer -= dt;
        if (transTimer > 0.f) return;

        switch (transState) {
        case TransitionState::FADE_OUT:
            transState = TransitionState::LOADING;
            transTimer = 0.5f;
            currentLevel = pendingLevel;
            break;
        case TransitionState::LOADING:
            transState = TransitionState::FADE_IN;
            transTimer = 1.0f;
            break;
        case TransitionState::FADE_IN:
            transState = TransitionState::NONE;
            pendingLevel = LevelID::NONE;
            break;
        default: break;
        }
    }

    float getFadeAlpha() const {
        switch (transState) {
        case TransitionState::FADE_OUT: return 1.f - transTimer / 1.0f;
        case TransitionState::LOADING:  return 1.f;
        case TransitionState::FADE_IN:  return transTimer / 1.0f;
        default: return 0.f;
        }
    }

    bool isTransitioning() const { return transState != TransitionState::NONE; }
};
