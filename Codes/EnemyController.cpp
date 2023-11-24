#include "EnemyController.h"
#include "GameGlobals.h"
#include "PlayerController.h"
#include "ProjectileManager.h"

#include <Scene.h>

#include <memory>
#include <cstdlib>

namespace ToolKit
{
  GameGlobals g_gameGlobals;

  // Movement State Machine
  //////////////////////////////////////////////////////////////////////////

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

  void EnemyWalkState::TransitionIn(State* prevState)
  {
    EnemyBaseState::TransitionIn(prevState);
    m_enemy->m_destionationReached = false; 
  }

  void EnemyWalkState::TransitionOut(State* nextState)
  {
    EnemyBaseState::TransitionOut(nextState);
    m_enemy->m_destionationReached = true; 
  }

  SignalId EnemyWalkState::Update(float deltaTime)
  {
    m_enemy->m_enemyPrefab->m_node->Translate(m_enemy->m_movementTargetDir * g_gameGlobals.m_enemyWalkSpeed * deltaTime);

    // destination is passed if the sign of any element for the vector (enemy pos -> target pos) is different than the direction
    const Vec3 currentPos = m_enemy->m_enemyPrefab->m_node->GetTranslation();
    if
    (
      GameUtils::Sgn(m_enemy->m_movementTargetPos.x - currentPos.x) != GameUtils::Sgn(m_enemy->m_movementTargetDir.x) ||
      GameUtils::Sgn(m_enemy->m_movementTargetPos.y - currentPos.y) != GameUtils::Sgn(m_enemy->m_movementTargetDir.y) ||
      GameUtils::Sgn(m_enemy->m_movementTargetPos.z - currentPos.z) != GameUtils::Sgn(m_enemy->m_movementTargetDir.z)
    )
    {
      // translate to the destination
      m_enemy->m_enemyPrefab->m_node->SetTranslation(m_enemy->m_movementTargetPos);

      return PlayerMovementSignal::Stop;
    }

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

  // Decision State Machine
  //////////////////////////////////////////////////////////////////////////

  String EnemyDecisionState::Null = "Null";
  String EnemyDecisionState::Patrol = "Patrol";
  String EnemyDecisionState::Attack = "Attack";

  void EnemyPatrolState::TransitionIn(State* prevState)
  {
    EnemyBaseState::TransitionIn(prevState);
    SetNextTarget();
  }

  SignalId EnemyPatrolState::Update(float deltaTime)
  {
    const Vec3Array patrolPoints = g_gameGlobals.m_enemyController->GetPatrolPoints();

    // patrol
    if (m_enemy->m_destionationReached)
    {
      SetNextTarget();
    }

    // if the player is visible, go attack state
    constexpr float angle = 0.707f; // (45 + 45) 90 degrees
    const Vec3 dir = glm::normalize(g_gameGlobals.m_playerController->m_playerPrefab->m_node->GetTranslation() - m_enemy->m_enemyPrefab->m_node->GetTranslation());
    if (glm::dot(m_enemy->m_enemyPrefab->GetComponent<DirectionComponent>()->GetDirection(), dir) > angle)
    {
      return EnemyDecisionSignal::Attack;
    }

    return NullSignal;
  }

  String EnemyPatrolState::Signaled(SignalId signal)
  {
    if (signal == EnemyDecisionSignal::Attack)
    {
      return EnemyDecisionState::Attack;
    }
    
    return EnemyDecisionState::Null;
  }

  void EnemyPatrolState::SetNextTarget()
  {
    m_enemy->SetMovementTarget(g_gameGlobals.m_enemyController->GetPatrolPoints()[m_enemy->m_lastPatrolIndex]);
    m_enemy->m_movementSM.Signal(EnemyMovementSignal::Move);
    m_enemy->m_lastPatrolIndex = (m_enemy->m_lastPatrolIndex + 1) % g_gameGlobals.m_enemyController->GetPatrolPoints().size();
  }

  SignalId EnemyAttackState::Update(float deltaTime)
  {
    // TODO better combat implementation can be implemented.

    // stop the movement
    m_enemy->m_movementSM.Signal(EnemyMovementSignal::Stop);

    // turn to the player
    const Vec3 pos =  m_enemy->m_enemyPrefab->m_node->GetTranslation();
    Vec3 dir = g_gameGlobals.m_playerController->m_playerPrefab->m_node->GetTranslation() - pos;
    Vec3 dirNorm = glm::normalize(dir);
    m_enemy->m_enemyPrefab->m_node->SetOrientation(GameUtils::QuatLookAtRH(dir));

    // move to the enemy
    if (glm::dot(dir, dir) > 25.0) // stop if player is near enough (5m)
    {
      m_enemy->m_enemyPrefab->m_node->Translate(dirNorm * g_gameGlobals.m_enemyWalkSpeed * deltaTime);
    }

    // fire projectiles
    static float pastTime = g_gameGlobals.m_enemyProjectileCooldown + 1.0f;
    pastTime += deltaTime;
    if (pastTime > g_gameGlobals.m_enemyProjectileCooldown)
    {
      pastTime -= g_gameGlobals.m_enemyProjectileCooldown;
      const Vec3 projectileStartPos = m_enemy->GetProjectileStartPos();
      g_gameGlobals.m_projectileManager->ShootProjectile
      (
        projectileStartPos,
        glm::normalize(g_gameGlobals.m_playerController->m_playerPrefab->m_node->GetTranslation() - projectileStartPos),
        g_gameGlobals.m_projectileSpeed, [](EntityPtr projectile, EntityPtr hit)
        {
          if (hit->GetTagVal() == "player")
          {
            //GetLogger()->WriteConsole(LogType::Command, "Player Hit");
          }
        }
      );
    }

    return NullSignal;
  }

  String EnemyAttackState::Signaled(SignalId signal)
  {
    if (signal == EnemyDecisionSignal::Patrol)
    {
      return EnemyDecisionState::Patrol;
    }

    return EnemyDecisionState::Null;
  }

  //////////////////////////////////////////////////////////////////////////

  Enemy::Enemy(EntityPtr prefab, EnemyController* controller)
  {
    m_enemyPrefab = prefab;
    m_health = 100.0f;
    m_enemyController = controller;
    m_movementTargetPos = ZERO;
    m_movementTargetDir = ZERO;
    m_destionationReached = false;

    // add direction component to enemy prefab
    m_enemyPrefab->AddComponent<DirectionComponent>();
  }

  Enemy::~Enemy()
  {
    g_gameGlobals.m_currentScene->RemoveEntity(m_enemyPrefab->GetIdVal());
    m_enemyPrefab = nullptr;
  }

  void Enemy::Update(float deltaTime)
  {
    m_decisionSM.Update(deltaTime);
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
    m_movementTargetPos = target;
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

  void EnemyController::Init(const ScenePtr scene)
  {
    // Accumulate patrol points in the scene
    bool noPoints = true;
    for (EntityPtr ntt : scene->GetByTag("pp"))
    {
      m_patrolPoints.push_back(ntt->m_node->GetTranslation());
      noPoints = false;
    }
    if (noPoints)
    {
      GetLogger()->WriteConsole(LogType::Warning, "No patrol points found in the scene! Using default two points.");
      m_patrolPoints.push_back(ZERO);
      m_patrolPoints.push_back(Vec3(5.0f, 0.75f, 5.0f));
    }

    // Fill the state machines of the enemies
    for (std::pair<ULongID, Enemy*> element : m_enemies)
    {
      FillEnemyStates(element.second);
    }
  }

  void EnemyController::FillEnemyStates(Enemy* enemy)
  {
      // movement states
      EnemyStationaryState* ss = new EnemyStationaryState(enemy);
      enemy->m_movementSM.PushState(ss);

      EnemyWalkState* ws = new EnemyWalkState(enemy);
      enemy->m_movementSM.PushState(ws);

      enemy->m_movementSM.m_currentState = ss;

      // decision states
      EnemyPatrolState* ps = new EnemyPatrolState(enemy);
      enemy->m_decisionSM.PushState(ps);

      EnemyAttackState* as = new EnemyAttackState(enemy);
      enemy->m_decisionSM.PushState(as);

      enemy->m_decisionSM.m_currentState = ps;

      ps->SetNextTarget(); // Set next target of the enemy
      // Note: State machine does not call TransitionIn() for the first state when Update() is called.
  }

  void EnemyController::AddEnemy(EntityPtr entity)
  {
    m_enemies[entity->GetIdVal()] = new Enemy(entity, this);
  }

  void EnemyController::Update(float deltaTime)
  {
    if (m_enemies.size() < g_gameGlobals.m_minEnemyCount)
    {
      SpawnEnemy();
    }

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

  void EnemyController::SpawnEnemy()
  {
    const float y = GameUtils::GetFloorY() + GameUtils::GetHalfPlayerHeight();
    const BoundingBox floorBB = GameUtils::GetFloorBB();
    const int height = static_cast<int>(floorBB.max.x - floorBB.min.x);
    const int width = static_cast<int>(floorBB.max.z - floorBB.min.z);
    const float x = (rand() % width) + floorBB.min.x;
    const float z = (rand() % height) + floorBB.min.z;
    const Vec3 pos = {x, y, z};

    EntityPtr newEnemyPrefab = GameUtils::EnemyPrefabInstantiate();
    newEnemyPrefab->SetTagVal("enemyPrefab");
    newEnemyPrefab->m_node->SetTranslation(pos);
    AddEnemy(newEnemyPrefab);
    FillEnemyStates(m_enemies[newEnemyPrefab->GetIdVal()]);
    m_enemies[newEnemyPrefab->GetIdVal()]->m_decisionSM.Signal(EnemyDecisionSignal::Attack);
  }
};
