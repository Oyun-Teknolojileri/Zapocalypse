#include "PlayerController.h"
#include "ProjectileManager.h"
#include "EnemyController.h"

namespace ToolKit
{
  String PlayerMovementState::Null = "";
  String PlayerMovementState::Stationary = "Stationary";
  String PlayerMovementState::Walk = "Walk";

  SignalId PlayerStationaryState::Update(float deltaTime)
  {
    if (g_gameGlobals.m_playerController->IsPlayerMoving())
    {
      return PlayerMovementSignal::Move;
    }

    g_gameGlobals.m_playerController->RotatePlayerWithMouse();

    return NullSignal;
  }

  String PlayerStationaryState::Signaled(SignalId signal)
  {
    if (signal == PlayerMovementSignal::Move)
    {
      return PlayerMovementState::Walk;
    }
    
    return PlayerMovementState::Null;
  }

  SignalId PlayerWalkState::Update(float deltaTime)
  {
    if (!g_gameGlobals.m_playerController->IsPlayerMoving())
    {
      return PlayerMovementSignal::Stop;
    }

    // Player rotation
    g_gameGlobals.m_playerController->RotatePlayerWithMouse();

    // Player movement

    const float speed = deltaTime * g_gameGlobals.m_playerWalkSpeed;
    static const Vec3 up = glm::normalize(Vec3(-1.0f, 0.0f, -1.0f));
    static const Vec3 down = glm::normalize(Vec3(1.0f, 0.0f, 1.0f));
    static const Vec3 left = glm::normalize(Vec3(-1.0f, 0.0f, 1.0f));
    static const Vec3 right = glm::normalize(Vec3(1.0f, 0.0f, -1.0f));

    if (g_gameGlobals.m_inputManager->WDown())
    {
      g_gameGlobals.m_playerController->m_playerPrefab->m_node->Translate(speed * up);
    }

    if (g_gameGlobals.m_inputManager->SDown())
    {
      g_gameGlobals.m_playerController->m_playerPrefab->m_node->Translate(speed * down);
    }

    if (g_gameGlobals.m_inputManager->ADown())
    {
      g_gameGlobals.m_playerController->m_playerPrefab->m_node->Translate(speed * left);
    }

    if (g_gameGlobals.m_inputManager->DDown())
    {
      g_gameGlobals.m_playerController->m_playerPrefab->m_node->Translate(speed * right);
    }

    return NullSignal;
  }

  String PlayerWalkState::Signaled(SignalId signal)
  {
    if (signal == PlayerMovementSignal::Stop)
    {
      return PlayerMovementState::Stationary;
    }

    return PlayerMovementState::Null;
  }

  String PlayerCombatState::Null = "";
  String PlayerCombatState::Hold = "Hold";
  String PlayerCombatState::Shoot = "Shoot";
  // TODO String PlayerCombatState::Reload;

  SignalId PlayerHoldState::Update(float deltaTime)
  {
    if (g_gameGlobals.m_playerController->IsPlayerTryingToShoot())
    {
      return PlayerCombatSignal::Shoot;
    }

    return NullSignal;
  }

  String PlayerHoldState::Signaled(SignalId signal)
  {
    if (signal == PlayerCombatSignal::Shoot)
    {
      return PlayerCombatState::Shoot;
    }

    return PlayerCombatState::Null;
  }

  SignalId PlayerShootState::Update(float deltaTime)
  {
    if (!g_gameGlobals.m_playerController->IsPlayerTryingToShoot())
    {
      return PlayerCombatSignal::Hold;
    }

    // TODO Shoot projectile based on rate of fire based on time (find a solution for how to shoot between frames when update misses the time of shoot (which will always happen))
    // Same implementation for enemy too.

    // Shoot
    static float pastTime = g_gameGlobals.m_projectileCooldown + 1.0f;
    pastTime += deltaTime;
    if (pastTime > g_gameGlobals.m_projectileCooldown)
    {
      pastTime -= g_gameGlobals.m_projectileCooldown;
      const Vec3 projectileStartPos = g_gameGlobals.m_playerController->GetProjectileStartPos();
      g_gameGlobals.m_projectileManager->ShootProjectile(projectileStartPos, glm::normalize(g_gameGlobals.m_playerController->m_pointOnPlane - projectileStartPos),
      g_gameGlobals.m_projectileSpeed, [](Entity* projectile, Entity* hit)
      {
        if (hit->GetTagVal() == "enemy")
        {
          constexpr float damage = 150.0f;
          g_gameGlobals.m_enemyController->HitEnemy(hit->m_node->m_parent->m_entity->GetIdVal(), damage);
          GetLogger()->WriteConsole(LogType::Command, "Enemy Hit");
        }
      });
    }

    return NullSignal;
  }

  String PlayerShootState::Signaled(SignalId signal)
  {
    if (signal == PlayerCombatSignal::Hold)
    {
      return PlayerCombatState::Hold;
    }

    return PlayerCombatState::Null;
  }

  void PlayerController::Init()
  {
    // movement states
    PlayerStationaryState* playerStationaryState = new PlayerStationaryState();
    m_PlayerMovementStateMachine.PushState(playerStationaryState);

    PlayerWalkState* playerWalkState = new PlayerWalkState();
    m_PlayerMovementStateMachine.PushState(playerWalkState);

    // Start with stationary state
    m_PlayerMovementStateMachine.m_currentState = playerStationaryState;

    // combat states
    PlayerHoldState* playerHoldState = new PlayerHoldState();
    m_PlayerCombatStateMachine.PushState(playerHoldState);

    PlayerShootState* playerShootState = new PlayerShootState();
    m_PlayerCombatStateMachine.PushState(playerShootState);

    // Start with hold state
    m_PlayerCombatStateMachine.m_currentState = playerHoldState;
  }

  void PlayerController::Update(float deltaTime)
  {
    // Create a plane little above floor and shoot ray from mouse to it. The intersecion point is used by multiple features.
    const PlaneEquation planeAboveFloor = GameUtils::GetPlaneAboveFloor();
    const Ray ray = GameUtils::GetRayFromMousePosition();
    float t;
    RayPlaneIntersection(ray, planeAboveFloor, t);
    if (t >= 0.0f)
    {
      m_pointOnPlaneValid = true;
      m_pointOnPlane = ray.position + ray.direction * t;
    }
    else
    {
      m_pointOnPlaneValid = false;
    }

    m_PlayerCombatStateMachine.Update(deltaTime);
    m_PlayerMovementStateMachine.Update(deltaTime);
  }
}
