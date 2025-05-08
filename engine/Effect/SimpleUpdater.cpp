#include "SimpleUpdater.h"
#include "operatorOverload.h"

void SimpleUpdater::Update(std::list<Particle>& particles, float deltaTime) {
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->currentTime += deltaTime;
        if (it->currentTime > it->lifeTime) {
            it = particles.erase(it);
        }
        else {
            it->transform.translate += it->velocity * deltaTime;
            ++it;
        }
    }
}