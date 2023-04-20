#include "PlayerController.h"

namespace ToolKit
{
  void PlayerController::Update(float deltaTime)
  {
    GetInputs();

    MovePlayer(deltaTime);
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

  void PlayerController::MovePlayer(float deltaTime)
  {
    const float speed = deltaTime * m_playerSpeed;
    static const Vec3 up = glm::normalize(Vec3(-1.0f, 0.0f, -1.0f));
    static const Vec3 down = glm::normalize(Vec3(1.0f, 0.0f, 1.0f));
    static const Vec3 left = glm::normalize(Vec3(-1.0f, 0.0f, 1.0f));
    static const Vec3 right = glm::normalize(Vec3(1.0f, 0.0f, -1.0f));

    if (m_moveUp)
    {
      m_player->m_node->Translate(speed * up);
    }

    if (m_moveDown)
    {
      m_player->m_node->Translate(speed * down);
    }

    if (m_moveLeft)
    {
      m_player->m_node->Translate(speed * left);
    }

    if (m_moveRight)
    {
      m_player->m_node->Translate(speed * right);
    }
  }
  
  void PlayerController::SetPlayerSpeed(float speed)
  {
    m_playerSpeed = speed;
  }
}
