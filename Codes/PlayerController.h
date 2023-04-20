#pragma once
#include "ToolKit.h"

namespace ToolKit
{
  class PlayerController
  {
    public:
      PlayerController(Entity* player) : m_player(player) {}
      ~PlayerController() {}

      void Update(float deltaTime);

      void SetPlayerSpeed(float speed);

    private:
      PlayerController() {}

      void GetInputs();
      void MovePlayer(float deltaTime);

    private:
      Entity* m_player = nullptr;

      bool m_moveUp = false;
      bool m_moveDown = false;
      bool m_moveRight = false;
      bool m_moveLeft = false;
      float m_playerSpeed = 0.1f;
  };
}
