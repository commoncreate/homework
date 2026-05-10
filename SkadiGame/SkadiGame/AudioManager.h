#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

class AudioManager {
public:
    static AudioManager& instance() {
        static AudioManager inst;
        return inst;
    }

    bool loadSound(const std::string& name, const std::string& path) {
        sf::SoundBuffer buf;
        if (!buf.loadFromFile(path)) {
            std::cerr << "[AUDIO] 加载失败: " << path << "\n";
            return false;
        }
        buffers[name] = std::move(buf);
        std::cout << "[AUDIO] 已加载: " << name << "\n";
        return true;
    }

    void playSound(const std::string& name, float volume = 100.f) {
        auto it = buffers.find(name);
        if (it == buffers.end()) {
            std::cerr << "[AUDIO] 音效未找到: " << name << "\n";
            return;
        }

        for (auto& s : pool) {
            if (s.getStatus() != sf::Sound::Playing) {
                s.setBuffer(it->second);
                s.setVolume(volume * masterVolume / 100.f);
                s.play();
                return;
            }
        }

        if (pool.size() < MAX_POOL_SIZE) {
            pool.emplace_back();
            pool.back().setBuffer(it->second);
            pool.back().setVolume(volume * masterVolume / 100.f);
            pool.back().play();
        }
    }

    bool playBGM(const std::string& path, float volume = 50.f) {
        if (!bgm.openFromFile(path)) {
            std::cerr << "[AUDIO] BGM 加载失败: " << path << "\n";
            return false;
        }
        bgm.setLoop(true);
        bgmBaseVolume = std::max(0.f, std::min(100.f, volume));
        bgm.setVolume(bgmBaseVolume * masterVolume / 100.f);
        bgm.play();
        std::cout << "[AUDIO] BGM 开始播放: " << path << "\n";
        return true;
    }

    void stopBGM() { bgm.stop(); }
    void pauseBGM() { bgm.pause(); }
    void resumeBGM() { bgm.play(); }
    bool isBGMPlaying() const {
        return bgm.getStatus() == sf::Music::Playing;
    }
    void setMasterVolume(float v) {
        masterVolume = std::max(0.f, std::min(100.f, v));
        // apply master volume relative to the base BGM volume (not the current scaled value)
        bgm.setVolume(bgmBaseVolume * masterVolume / 100.f);
    }
    float getMasterVolume() const { return masterVolume; }

private:
    AudioManager() { pool.reserve(MAX_POOL_SIZE); }

    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::vector<sf::Sound> pool;      
    sf::Music bgm;                     
    float masterVolume = 100.f;
    float bgmBaseVolume = 50.f; // store the last requested BGM volume
    static constexpr size_t MAX_POOL_SIZE = 32;
};