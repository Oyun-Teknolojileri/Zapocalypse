#pragma once

#include "Types.h"

namespace ToolKit
{
  extern class GameGlobals g_gameGlobals;

  class GameGlobals
  {
    public:
    // Global managers and controllers
    class PlayerController* m_playerController = nullptr;
    class EnemyController* m_enemyController = nullptr;
    class InputManager* m_inputManager = nullptr;
    class ProjectileManager* m_projectileManager = nullptr;
    ScenePtr m_currentScene = nullptr;
  };
}