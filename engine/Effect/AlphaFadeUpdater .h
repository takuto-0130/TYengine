#pragma once
#include "IUpdater.h"
#include "operatorOverload.h"

class AlphaFadeUpdater : public IUpdater {
public:
    void Update(std::list<Particle>& particles, float deltaTime) override {
        for (auto it = particles.begin(); it != particles.end(); ) {
            it->currentTime += deltaTime;
            if (it->currentTime > it->lifeTime) {
                it = particles.erase(it);
            }
            else {
                it->transform.translate += it->velocity * deltaTime;
                float alpha = 1.0f - (it->currentTime / it->lifeTime);
                it->color.w = alpha;
                ++it;
            }
        }
    }
};