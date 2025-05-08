#pragma once
#include "IEmitter.h"
#include "struct.h"

class ConeEmitter : public IEmitter {
public:
    Transform transform;
    uint32_t count = 10;

    std::list<Particle> Emit(std::mt19937& rng) override;
};

