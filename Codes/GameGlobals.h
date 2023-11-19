#pragma once

#include <Types.h>

namespace ToolKit
{
  extern class GameGlobals g_gameGlobals;
  typedef std::shared_ptr<class PlayerController> PlayerControllerPtr;
  typedef std::shared_ptr<class EnemyController> EnemyControllerPtr;
  typedef std::shared_ptr<class InputManager> InputManagerPtr;
  typedef std::shared_ptr<class ProjectileManager> ProjectileManagerPtr;

  class  GameGlobals
  {
    public:
    // Global managers and controllers
    PlayerControllerPtr m_playerController = nullptr;
    EnemyControllerPtr m_enemyController = nullptr;
    InputManagerPtr m_inputManager = nullptr;
    ProjectileManagerPtr m_projectileManager = nullptr;
    ScenePtr m_currentScene = nullptr;

    float m_enemyWalkSpeed = 0.002f;
    float m_playerWalkSpeed = 0.01f;
    float m_projectileSpeed = 0.05f;
    float m_playerProjectileCooldown = 500.0f;
    float m_enemyProjectileCooldown = 1000.0f;
    int m_minEnemyCount = 10;
  };
}