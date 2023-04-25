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

    if (m_inputManager->WDown())
    {
      m_playerController->m_player->m_node->Translate(speed * up);
    }

    if (m_inputManager->SDown())
    {
      m_playerController->m_player->m_node->Translate(speed * down);
    }

    if (m_inputManager->ADown())
    {
      m_playerController->m_player->m_node->Translate(speed * left);
    }

    if (m_inputManager->DDown())
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
    idleState->m_inputManager = m_inputManager;
    m_stateMachine.PushState(idleState);

    PlayerWalkState* walkState = new PlayerWalkState();
    walkState->m_playerController = this;
    walkState->m_inputManager = m_inputManager;
    m_stateMachine.PushState(walkState);

    // Start with idle state
    m_stateMachine.m_currentState = idleState;
  }

  void PlayerController::Update(float deltaTime)
  {
    m_stateMachine.Update(deltaTime);
  }
}
