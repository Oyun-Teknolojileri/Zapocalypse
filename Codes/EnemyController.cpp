#include "EnemyController.h"
#include "GameGlobals.h"

namespace ToolKit
{
  GameGlobals g_gameGlobals;

  Enemy::Enemy(Entity* prefab, EnemyController* controller)
  {
    m_enemyPrefab = prefab;
    m_health = 100.0f;
    m_enemyController = controller;
  }

  Enemy::~Enemy()
  {
    g_gameGlobals.m_currentScene->RemoveEntity(m_enemyPrefab->GetIdVal());
    delete m_enemyPrefab;
  }

  void Enemy::Update(float deltaTime)
  {

  }

  void Enemy::GetHit(float damageAmount)
  {
    m_health -= damageAmount;

    GetLogger()->WriteConsole(LogType::Warning, "HP: %f.", m_health);

    if (m_health <= 0.0f)
    {
      Die();
    }
  }

  void Enemy::Die()
  {
    GetLogger()->WriteConsole(LogType::Warning, "Die");
    m_enemyController->KillEnemy(m_enemyPrefab->GetIdVal());
  }

  EnemyController::~EnemyController()
  {
    for (std::pair<ULongID, Enemy*> element : m_enemies)
    {
      delete element.second;
    }
    m_enemies.clear();
  }

  void EnemyController::AddEnemy(Entity* entity)
  {
    m_enemies[entity->GetIdVal()] = new Enemy(entity, this);
  }

  void EnemyController::Update(float deltaTime)
  {
    for (std::pair<ULongID, Enemy*> element : m_enemies)
    {
      element.second->Update(deltaTime);
    }
  }

  void EnemyController::HitEnemy(ULongID id, float damageAmount)
  {
    if (m_enemies.find(id) != m_enemies.end())
    {
      m_enemies[id]->GetHit(damageAmount);
    }
  }

  void EnemyController::KillEnemy(ULongID id)
  {
    std::unordered_map<ULongID, Enemy*>::iterator it = m_enemies.find(id);
    if (it != m_enemies.end())
    {
      Enemy* enemy = m_enemies[id];
      m_enemies.erase(it);
      delete enemy;
    }
  }
};
