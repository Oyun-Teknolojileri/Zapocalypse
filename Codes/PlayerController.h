#pragma once
#include "ToolKit.h"
#include "StateMachine.h"
#include "InputManager.h"

namespace ToolKit
{
  class PlayerSignal
  {
   public:
    static SignalId Move = 1;
    static SignalId Stop = 2;
  };

  class PlayerState
  {
   public:
    static String Null;
    static String Idle;
    static String Walk;
  };

  class PlayerBaseState : public State
  {
   public:
    PlayerBaseState() {}
    virtual ~PlayerBaseState() {}
    void TransitionIn(State* prevState) override {}
    void TransitionOut(State* nextState) override {}

   public:
    class PlayerController* m_playerController = nullptr;
    InputManager* m_inputManager = nullptr;
  };

  class PlayerIdleState : public PlayerBaseState
  {
   public:
    PlayerIdleState() {}
    virtual ~PlayerIdleState() {}

    void TransitionIn(State* prevState) override;
    void TransitionOut(State* nextState) override;
    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerState::Idle; }
  };

  class PlayerWalkState : public PlayerBaseState
  {
   public:
    PlayerWalkState() {}
    virtual ~PlayerWalkState() {}

    void TransitionIn(State* prevState) override;
    void TransitionOut(State* nextState) override;
    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return PlayerState::Walk; }
  };

  class PlayerController
  {
     public:
      PlayerController(Entity* player, InputManager* inputManager) : m_player(player), m_inputManager(inputManager) {}
      ~PlayerController() {}

      void Init();
      void Update(float deltaTime);

      inline bool IsPlayerMoving()
      {
        return m_inputManager->WDown() || m_inputManager->SDown() || m_inputManager->ADown() || m_inputManager->DDown();
      }

     private:
      PlayerController() {}

     public:
      StateMachine m_stateMachine;

      Entity* m_player = nullptr;
      InputManager* m_inputManager = nullptr;

      float m_playerWalkSpeed = 0.1f;
  };
}
