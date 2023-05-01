#pragma once
#include "ToolKit.h"
#include "StateMachine.h"
#include "InputManager.h"
#include "ProjectileManager.h"

namespace ToolKit
{
  class BaseState : public State
  {
   public:
    BaseState() {}
    virtual ~BaseState() {}
    void TransitionIn(State* prevState) override {}
    void TransitionOut(State* nextState) override {}

   public:
    class PlayerController* m_playerController = nullptr;
    InputManager* m_inputManager = nullptr;
  };

  // Movement State Machine
  //////////////////////////////////////////////////////////////////////////

  class MovementSignal
  {
   public:
    static SignalId Move = 1;
    static SignalId Stop = 2;
  };

  class MovementState
  {
   public:
    static String Null;
    static String Stationary;
    static String Walk;
  };

  class StationaryState : public BaseState
  {
   public:
    StationaryState() {}
    virtual ~StationaryState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return MovementState::Stationary; }
  };

  class WalkState : public BaseState
  {
   public:
    WalkState() {}
    virtual ~WalkState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return MovementState::Walk; }
  };

  // Combat State Machine
  //////////////////////////////////////////////////////////////////////////

  class CombatSignal
  {
   public:
    static SignalId Hold = 1;
    static SignalId Shoot = 2;
    // TODO static SignalId Reload = 3;
  };

  class CombatState
  {
   public:
    static String Null;
    static String Hold;
    static String Shoot;
    // TODO static String Reload;
  };

  class HoldState : public BaseState
  {
   public:
    HoldState() {}
    virtual ~HoldState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return CombatState::Hold; }
  };

  class ShootState : public BaseState
  {
   public:
    ShootState() {}
    virtual ~ShootState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return CombatState::Shoot; }
  };

  //////////////////////////////////////////////////////////////////////////

  class PlayerController
  {
     public:
      PlayerController(Entity* playerPrefab, InputManager* inputManager, ProjectileManager* projectileManager)
        : m_playerPrefab(playerPrefab), m_inputManager(inputManager), m_projectileManager(projectileManager) {}
      ~PlayerController() {}

      void Init();
      void Update(float deltaTime);

      inline bool IsPlayerMoving()
      {
        return m_inputManager->WDown() || m_inputManager->SDown() || m_inputManager->ADown() || m_inputManager->DDown();
      }

      inline bool IsPlayerTryingToShoot()
      {
        return m_inputManager->LeftMouseDown();
      }

     private:
      PlayerController() {}

     public:
      StateMachine m_movementStateMachine;
      StateMachine m_combatStateMachine;

      Entity* m_playerPrefab = nullptr;
      InputManager* m_inputManager = nullptr;
      ProjectileManager* m_projectileManager = nullptr;

      float m_playerWalkSpeed = 0.1f;

      Vec3 m_projectileStartPos = ZERO;
      float m_projectileSpeed = 0.05f;
      float m_projectileCooldown = 200.0f;

      ScenePtr m_scene = nullptr;
  };
}
