#pragma once

#include <unordered_map>
#include "Toolkit.h"

namespace ToolKit
{
  class Enemy
  {
    friend class EnemyController;

    public:
    Enemy(Entity* entity, class EnemyController* controller);
    ~Enemy();

    void Update(float deltaTime);
    void GetHit(float damageAmount);
    void Die();

    private:
    Enemy() {}

    private:
    Entity* m_enemyPrefab = nullptr;
    float m_health;

    EnemyController* m_enemyController = nullptr;
  };

  class EnemyController
  {
    public:
    ~EnemyController();

    void AddEnemy(Entity* entity);
    void Update(float deltaTime);
    void HitEnemy(ULongID id, float damageAmount);
    void KillEnemy(ULongID id);

    private:
    std::unordered_map<ULongID, Enemy*> m_enemies;
  };
};