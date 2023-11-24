#pragma once

#include "GameUtils.h"
#include "GameGlobals.h"
#include "InputManager.h"

#include <ToolKit.h>
#include <StateMachine.h>
#include <MathUtil.h>
#include <DirectionComponent.h>

namespace ToolKit
{
  class  PlayerBaseState : public State
  {
   public:
    PlayerBaseState() {}
    virtual ~PlayerBaseState() {}
    void TransitionIn(State* prevState) override {}
    void TransitionOut(State* nextState) override {}
  };

  // Movement State Machine
  //////////////////////////////////////////////////////////////////////////

  class  PlayerMovementSignal
  {
   public:
    static SignalId Move = 1;
    static SignalId Stop = 2;
  };

  class  PlayerMovementState
  {
   public:
    static String Null;
    static String Stationary;
    static String Walk;
  };

  class  PlayerStationaryState : public PlayerBaseState
  {
   public:
    PlayerStationaryState() {}
    virtual ~PlayerStationaryState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerMovementState::Stationary; }
  };

  class  PlayerWalkState : public PlayerBaseState
  {
   public:
    PlayerWalkState() {}
    virtual ~PlayerWalkState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerMovementState::Walk; }
  };

  // Combat State Machine
  //////////////////////////////////////////////////////////////////////////

  class  PlayerCombatSignal
  {
   public:
    static SignalId Hold = 1;
    static SignalId Shoot = 2;
    // TODO static SignalId Reload = 3;
  };

  class  PlayerCombatState
  {
   public:
    static String Null;
    static String Hold;
    static String Shoot;
    // TODO static String Reload;
  };

  class  PlayerHoldState : public PlayerBaseState
  {
   public:
    PlayerHoldState() {}
    virtual ~PlayerHoldState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerCombatState::Hold; }
  };

  class  PlayerShootState : public PlayerBaseState
  {
   public:
    PlayerShootState() {}
    virtual ~PlayerShootState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerCombatState::Shoot; }
  };

  //////////////////////////////////////////////////////////////////////////

  class  PlayerController
  {
     public:
      PlayerController(EntityPtr playerPrefab) : m_playerPrefab(playerPrefab) {}
      ~PlayerController() {}

      void Init();
      void Update(float deltaTime);

      inline bool IsPlayerMoving()
      {
        bool dpadDeltaX = fabs(g_gameGlobals.m_inputManager->DpadX()) > 0.001f;
        bool dpadDeltaY = fabs(g_gameGlobals.m_inputManager->DpadY()) > 0.001f;

        return g_gameGlobals.m_inputManager->WDown() || g_gameGlobals.m_inputManager->SDown() || g_gameGlobals.m_inputManager->ADown() ||
        g_gameGlobals.m_inputManager->DDown() || dpadDeltaX || dpadDeltaY;
      }

      inline bool IsPlayerTryingToShoot()
      {
        return g_gameGlobals.m_inputManager->LeftMouseDown();
      }

      inline Vec3 GetProjectileStartPos()
      {
        static Node* projectileStartPosNode = nullptr;
        if (!projectileStartPosNode)
        {
          for (Node* node : m_playerPrefab->m_node->m_children[0]->m_children)
          {
            if (node->OwnerEntity()->GetTagVal() == "playerMuzzlePos")
            {
              projectileStartPosNode = node;
              break;
            }
          }
        }
        return projectileStartPosNode->GetTranslation();
      }

      inline void RotatePlayerWithMouse()
      {
        if (m_pointOnPlaneValid)
        {
          m_playerPrefab->m_node->SetOrientation(GameUtils::QuatLookAtRH(m_pointOnPlane - m_playerPrefab->m_node->GetTranslation()));
        }
      }

     private:
      PlayerController() {}

     public:
      StateMachine m_PlayerMovementStateMachine;
      StateMachine m_PlayerCombatStateMachine;

      EntityPtr m_playerPrefab = nullptr;

      bool m_pointOnPlaneValid = false;
      Vec3 m_pointOnPlane = ZERO;

      ScenePtr m_scene = nullptr;
  };
}
