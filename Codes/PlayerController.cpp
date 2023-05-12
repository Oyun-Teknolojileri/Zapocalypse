#include "PlayerController.h"
#include "ProjectileManager.h"
#include "EnemyController.h"

namespace ToolKit
{
  String MovementState::Null = "";
  String MovementState::Stationary = "Stationary";
  String MovementState::Walk = "Walk";

  SignalId StationaryState::Update(float deltaTime)
  {
    if (g_gameGlobals.m_playerController->IsPlayerMoving())
    {
      return MovementSignal::Move;
    }

    g_gameGlobals.m_playerController->RotatePlayerWithMouse();

    return NullSignal;
  }

  String StationaryState::Signaled(SignalId signal)
  {
    if (signal == MovementSignal::Move)
    {
      return MovementState::Walk;
    }
    
    return MovementState::Null;
  }

  SignalId WalkState::Update(float deltaTime)
  {
    if (!g_gameGlobals.m_playerController->IsPlayerMoving())
    {
      return MovementSignal::Stop;
    }

    // Player rotation
    g_gameGlobals.m_playerController->RotatePlayerWithMouse();

    // Player movement

    const float speed = deltaTime * g_gameGlobals.m_playerController->m_playerWalkSpeed;
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

  String WalkState::Signaled(SignalId signal)
  {
    if (signal == MovementSignal::Stop)
    {
      return MovementState::Stationary;
    }

    return MovementState::Null;
  }

  String CombatState::Null = "";
  String CombatState::Hold = "Hold";
  String CombatState::Shoot = "Shoot";
  // TODO String CombatState::Reload;

  SignalId HoldState::Update(float deltaTime)
  {
    if (g_gameGlobals.m_playerController->IsPlayerTryingToShoot())
    {
      return CombatSignal::Shoot;
    }

    return NullSignal;
  }

  String HoldState::Signaled(SignalId signal)
  {
    if (signal == CombatSignal::Shoot)
    {
      return CombatState::Shoot;
    }

    return CombatState::Null;
  }

  SignalId ShootState::Update(float deltaTime)
  {
    if (!g_gameGlobals.m_playerController->IsPlayerTryingToShoot())
    {
      return CombatSignal::Hold;
    }

    // TODO Shoot projectile based on rate of fire based on time (find a solution for how to shoot between frames when update misses the time of shoot (which will always happen))

    // Shoot
    static float pastTime = g_gameGlobals.m_playerController->m_projectileCooldown + 1.0f;
    pastTime += deltaTime;
    if (pastTime > g_gameGlobals.m_playerController->m_projectileCooldown)
    {
      pastTime -= g_gameGlobals.m_playerController->m_projectileCooldown;
      const Vec3 projectileStartPos = g_gameGlobals.m_playerController->GetProjectileStartPos();
      g_gameGlobals.m_projectileManager->ShootProjectile(projectileStartPos, glm::normalize(g_gameGlobals.m_playerController->m_pointOnPlane - projectileStartPos),
      g_gameGlobals.m_playerController->m_projectileSpeed, [](Entity* projectile, Entity* hit)
      {
        const char* projectileName = projectile->GetNameVal().c_str();
        const char* hitName = hit->GetNameVal().c_str();
        if (hit->GetTagVal() == "enemy")
        {
          g_gameGlobals.m_enemyController->HitEnemy(hit->m_node->m_parent->m_entity->GetIdVal(), 50.0f);
        }
        GetLogger()->WriteConsole(LogType::Warning, "%s shoots %s.", projectileName, hitName);
      });
    }

    return NullSignal;
  }

  String ShootState::Signaled(SignalId signal)
  {
    if (signal == CombatSignal::Hold)
    {
      return CombatState::Hold;
    }

    return CombatState::Null;
  }

  void PlayerController::Init()
  {
    // movement states
    StationaryState* stationaryState = new StationaryState();
    m_movementStateMachine.PushState(stationaryState);

    WalkState* walkState = new WalkState();
    m_movementStateMachine.PushState(walkState);

    // Start with stationary state
    m_movementStateMachine.m_currentState = stationaryState;

    // combat states
    HoldState* holdState = new HoldState();
    m_combatStateMachine.PushState(holdState);

    ShootState* shootState = new ShootState();
    m_combatStateMachine.PushState(shootState);

    // Start with hold state
    m_combatStateMachine.m_currentState = holdState;
  }

  void PlayerController::Update(float deltaTime)
  {
    // Create a plane little above floor and shoot ray from mouse to it. The intersecion point is used by multiple features.
    static const PlaneEquation planeAboveFloor =
    {Vec3(0.0f, 1.0f, 0.0f), GameUtils::GetFloorY() + m_playerPrefab->m_node->m_children[0]->m_entity->GetMeshComponent()->GetAABB().GetHeight() / 2.0f};
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

    m_combatStateMachine.Update(deltaTime);
    m_movementStateMachine.Update(deltaTime);
  }
}
