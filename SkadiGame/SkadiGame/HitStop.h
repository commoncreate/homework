#pragma once

class HitStop {
public:
    static HitStop& instance() { static HitStop h; return h; }

    void freeze(float duration) {
        if (duration > remaining) remaining = duration;
    }

    bool isFrozen() const { return remaining > 0.f; }

    void tick(float realDt) {
        if (remaining > 0.f) remaining -= realDt;
    }

private:
    HitStop() = default;
    float remaining = 0.f;
};
