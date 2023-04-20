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

    private:
      PlayerController() {}

    private:
      Entity* m_player = nullptr;
  };
}