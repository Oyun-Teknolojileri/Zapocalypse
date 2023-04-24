#include "PlayerController.h"

namespace ToolKit
{
  String PlayerState::Null = "";
  String PlayerState::Idle = "Idle";
  String PlayerState::Walk = "Walk";

  void PlayerIdleState::TransitionIn(State* prevState)
  {
    PlayerBaseState::TransitionIn(prevState);
  }

  void PlayerIdleState::TransitionOut(State* nextState)
  {
    PlayerBaseState::TransitionOut(nextState);
  }

  SignalId PlayerIdleState::Update(float deltaTime)
  {
    if (m_playerController->IsPlayerMoving())
    {
      return PlayerSignal::Move;
    }

    return NullSignal;
  }

  String PlayerIdleState::Signaled(SignalId signal)
  {
    if (signal == PlayerSignal::Move)
    {
      return PlayerState::Walk;
    }
    
    return PlayerState::Null;
  }

  void PlayerWalkState::TransitionIn(State* prevState)
  {
    PlayerBaseState::TransitionIn(prevState);
  }

  void PlayerWalkState::TransitionOut(State* nextState)
  {
    PlayerBaseState::TransitionOut(nextState);
  }

  SignalId PlayerWalkState::Update(float deltaTime)
  {
    if (!m_playerController->IsPlayerMoving())
    {
      return PlayerSignal::Stop;
    }

    // Player movement

    const float speed = deltaTime * m_playerController->m_playerWalkSpeed;
    static const Vec3 up = glm::normalize(Vec3(-1.0f, 0.0f, -1.0f));
    static const Vec3 down = glm::normalize(Vec3(1.0f, 0.0f, 1.0f));
    static const Vec3 left = glm::normalize(Vec3(-1.0f, 0.0f, 1.0f));
    static const Vec3 right = glm::normalize(Vec3(1.0f, 0.0f, -1.0f));

    if (m_playerController->MoveUp())
    {
      m_playerController->m_player->m_node->Translate(speed * up);
    }

    if (m_playerController->MoveDown())
    {
      m_playerController->m_player->m_node->Translate(speed * down);
    }

    if (m_playerController->MoveLeft())
    {
      m_playerController->m_player->m_node->Translate(speed * left);
    }

    if (m_playerController->MoveRight())
    {
      m_playerController->m_player->m_node->Translate(speed * right);
    }

    return NullSignal;
  }

  String PlayerWalkState::Signaled(SignalId signal)
  {
    if (signal == PlayerSignal::Stop)
    {
      return PlayerState::Walk;
    }

    return PlayerState::Null;
  }

  void PlayerController::Init()
  {
    PlayerIdleState* idleState = new PlayerIdleState();
    idleState->m_playerController = this;
    m_stateMachine.PushState(idleState);

    PlayerWalkState* walkState = new PlayerWalkState();
    walkState->m_playerController = this;
    m_stateMachine.PushState(walkState);

    // Start with idle state
    m_stateMachine.m_currentState = idleState;
  }

  void PlayerController::Update(float deltaTime)
  {
    GetInputs();

    m_stateMachine.Update(deltaTime);
  }

  void PlayerController::GetInputs()
  {
    EventPool& events = Main::GetInstance()->m_eventPool;
    for (Event* event : events)
    {
      if (event->m_type == Event::EventType::Keyboard)
      {
        KeyboardEvent* ke = static_cast<KeyboardEvent*>(event);

        if (ke->m_action == EventAction::KeyDown)
        {
          if (ke->m_keyCode == 'w')
          {
            m_moveUp = true;
          }
          else if (ke->m_keyCode == 's')
          {
            m_moveDown = true;
          }
          else if (ke->m_keyCode == 'a')
          {
            m_moveLeft = true;
          }
          else if (ke->m_keyCode == 'd')
          {
            m_moveRight = true;
          }
        }

        if (ke->m_action == EventAction::KeyUp)
        {
          if (ke->m_keyCode == 'w')
          {
            m_moveUp = false;
          }
          else if (ke->m_keyCode == 's')
          {
            m_moveDown = false;
          }
          else if (ke->m_keyCode == 'a')
          {
            m_moveLeft = false;
          }
          else if (ke->m_keyCode == 'd')
          {
            m_moveRight = false;
          }
        }
      }
    }
  }
}
