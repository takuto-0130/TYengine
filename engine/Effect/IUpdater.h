#pragma once
#include "Particle.h"
#include <list>

class IUpdater {
public:
    virtual ~IUpdater() = default;
    virtual void Update(std::list<Particle>& particles, float deltaTime) = 0;
};
