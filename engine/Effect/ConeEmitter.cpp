#include "ConeEmitter.h"
#include <random>

std::list<Particle> ConeEmitter::Emit(std::mt19937& rng) {
    std::list<Particle> particles;
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> life(1.0f, 3.0f);

    for (uint32_t i = 0; i < count; ++i) {
        Particle p;
        p.transform = transform;
        p.transform.translate += { dist(rng), dist(rng), dist(rng) };
        p.velocity = { dist(rng), dist(rng), dist(rng) };
        p.color = { 1, 1, 1, 1 };
        p.lifeTime = life(rng);
        p.currentTime = 0;
        particles.push_back(p);
    }
    return particles;
}
