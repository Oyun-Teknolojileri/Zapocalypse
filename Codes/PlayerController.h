#pragma once
#include "ToolKit.h"
#include "StateMachine.h"

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
      PlayerController(Entity* player) : m_player(player) {}
      ~PlayerController() {}

      void Init();
      void Update(float deltaTime);

      inline bool MoveUp()
      {
        return m_moveUp;
      }
      inline bool MoveDown()
      {
        return m_moveDown;
      }
      inline bool MoveLeft()
      {
        return m_moveLeft;
      }
      inline bool MoveRight()
      {
        return m_moveRight;
      }
      inline bool IsPlayerMoving()
      {
        return m_moveUp || m_moveDown || m_moveLeft || m_moveRight;
      }

     private:
      PlayerController() {}

      void GetInputs();

     public:
      StateMachine m_stateMachine;

      Entity* m_player = nullptr;

      float m_playerWalkSpeed = 0.1f;

     private:
      bool m_moveUp = false;
      bool m_moveDown = false;
      bool m_moveRight = false;
      bool m_moveLeft = false;
  };
}
