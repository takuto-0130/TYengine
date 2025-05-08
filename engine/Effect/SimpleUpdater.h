#pragma once
#include "IUpdater.h"

class SimpleUpdater : public IUpdater {
public:
    void Update(std::list<Particle>& particles, float deltaTime) override;
};
