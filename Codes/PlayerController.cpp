#include "PlayerController.h"

namespace ToolKit
{
  String MovementState::Null = "";
  String MovementState::Stationary = "Stationary";
  String MovementState::Walk = "Walk";

  SignalId StationaryState::Update(float deltaTime)
  {
    if (m_playerController->IsPlayerMoving())
    {
      return MovementSignal::Move;
    }

    m_playerController->RotatePlayerWithMouse();

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
    if (!m_playerController->IsPlayerMoving())
    {
      return MovementSignal::Stop;
    }

    // Player rotation
    m_playerController->RotatePlayerWithMouse();

    // Player movement

    const float speed = deltaTime * m_playerController->m_playerWalkSpeed;
    static const Vec3 up = glm::normalize(Vec3(-1.0f, 0.0f, -1.0f));
    static const Vec3 down = glm::normalize(Vec3(1.0f, 0.0f, 1.0f));
    static const Vec3 left = glm::normalize(Vec3(-1.0f, 0.0f, 1.0f));
    static const Vec3 right = glm::normalize(Vec3(1.0f, 0.0f, -1.0f));

    if (m_inputManager->WDown())
    {
      m_playerController->m_playerPrefab->m_node->Translate(speed * up);
    }

    if (m_inputManager->SDown())
    {
      m_playerController->m_playerPrefab->m_node->Translate(speed * down);
    }

    if (m_inputManager->ADown())
    {
      m_playerController->m_playerPrefab->m_node->Translate(speed * left);
    }

    if (m_inputManager->DDown())
    {
      m_playerController->m_playerPrefab->m_node->Translate(speed * right);
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
    if (m_playerController->IsPlayerTryingToShoot())
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
    if (!m_playerController->IsPlayerTryingToShoot())
    {
      return CombatSignal::Hold;
    }

    // Shoot
    static float pastTime = m_playerController->m_projectileCooldown + 1.0f;
    pastTime += deltaTime;
    if (pastTime > m_playerController->m_projectileCooldown)
    {
      pastTime = 0.0f;

      Ray rayToScene = GameUtils::GetRayFromMousePosition();
      Scene::PickData pickData = m_playerController->m_scene->PickObject(rayToScene, {});
      if (pickData.entity)
      {
        const Vec3 projectileStartPos = m_playerController->GetProjectileStartPos();
        m_playerController->m_projectileManager->ShootProjectile(projectileStartPos, glm::normalize(m_playerController->m_pointOnPlane - projectileStartPos),
        m_playerController->m_projectileSpeed, [](Entity* object, Entity* hit)
        {
          GetLogger()->WriteConsole(LogType::Warning, "%s shoots %s.", object->GetNameVal().c_str(), hit->GetNameVal().c_str());
        });
      }
    }

    // TODO Shoot projectile based on rate of fire based on time (find a solution for how to shoot between frames when update misses the time of shoot (which probably is gonna happen always))

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
    stationaryState->m_playerController = this;
    stationaryState->m_inputManager = m_inputManager;
    m_movementStateMachine.PushState(stationaryState);

    WalkState* walkState = new WalkState();
    walkState->m_playerController = this;
    walkState->m_inputManager = m_inputManager;
    m_movementStateMachine.PushState(walkState);

    // Start with stationary state
    m_movementStateMachine.m_currentState = stationaryState;

    // combat states
    HoldState* holdState = new HoldState();
    holdState->m_playerController = this;
    holdState->m_inputManager = m_inputManager;
    m_combatStateMachine.PushState(holdState);

    ShootState* shootState = new ShootState();
    shootState->m_playerController = this;
    shootState->m_inputManager = m_inputManager;
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
