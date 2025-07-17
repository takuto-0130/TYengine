#pragma once

class Enemy;

class IEnemyEventListener {
public:
    virtual ~IEnemyEventListener() = default;

    virtual void OnEnemyDied(Enemy* enemy) = 0;
};