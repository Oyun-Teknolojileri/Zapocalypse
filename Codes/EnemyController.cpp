#include "EnemyController.h"
#include "GameGlobals.h"
#include "PlayerController.h"

namespace ToolKit
{
  GameGlobals g_gameGlobals;

  String EnemyMovementState::Null = "";
  String EnemyMovementState::Stationary = "Stationary";
  String EnemyMovementState::Walk = "Walk";

  SignalId EnemyStationaryState::Update(float deltaTime)
  {
    return NullSignal;
  }

  String EnemyStationaryState::Signaled(SignalId signal)
  {
    if (signal == EnemyMovementSignal::Move)
    {
      return EnemyMovementState::Walk;
    }
    
    return EnemyMovementState::Null;
  }

  SignalId EnemyWalkState::Update(float deltaTime)
  {
    m_enemy->m_enemyPrefab->m_node->Translate(m_enemy->m_movementTargetDir * g_gameGlobals.m_enemyController->m_enemyWalkSpeed * deltaTime);
    
    return NullSignal;
  }

  String EnemyWalkState::Signaled(SignalId signal)
  {
    if (signal == EnemyMovementSignal::Stop)
    {
      return EnemyMovementState::Stationary;
    }

    return EnemyMovementState::Null;
  }

  Enemy::Enemy(Entity* prefab, EnemyController* controller)
  {
    m_enemyPrefab = prefab;
    m_health = 100.0f;
    m_enemyController = controller;
    m_movementTargetPos = ZERO;
    m_movementTargetDir = ZERO;
  }

  Enemy::~Enemy()
  {
    g_gameGlobals.m_currentScene->RemoveEntity(m_enemyPrefab->GetIdVal());
    delete m_enemyPrefab;
  }

  void Enemy::Update(float deltaTime)
  {
    m_movementSM.Update(deltaTime);
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

  void Enemy::SetMovementTarget(const Vec3 target)
  {
    const Vec3 pos = m_enemyPrefab->m_node->GetTranslation();
    m_movementTargetDir = glm::normalize(target - pos);
    m_enemyPrefab->m_node->SetOrientation(GameUtils::QuatLookAtRH(m_movementTargetDir));
  }

  EnemyController::~EnemyController()
  {
    for (std::pair<ULongID, Enemy*> element : m_enemies)
    {
      delete element.second;
    }
    m_enemies.clear();
  }

  void EnemyController::Init()
  {
    // Fill the state machines of the enemies
    for (std::pair<ULongID, Enemy*> element : m_enemies)
    {
      Enemy* enemy = element.second;
      EnemyStationaryState* ess = new EnemyStationaryState(enemy);
      enemy->m_movementSM.PushState(ess);

      EnemyWalkState* ews = new EnemyWalkState(enemy);
      enemy->m_movementSM.PushState(ews);

      enemy->m_movementSM.m_currentState = ess;
    }
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
