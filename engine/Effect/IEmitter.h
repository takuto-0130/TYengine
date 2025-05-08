#pragma once
#include "Particle.h"
#include <list>
#include <random>

class IEmitter {
public:
    virtual ~IEmitter() = default;
    virtual std::list<Particle> Emit(std::mt19937& rng) = 0;
};
