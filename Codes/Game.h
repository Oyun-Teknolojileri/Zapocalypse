#pragma once

#include "ToolKit.h"
#include "Plugin.h"
#include "SceneRenderer.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "ProjectileManager.h"
#include "EnemyController.h"

namespace ToolKit
{
  class Game : public GamePlugin
  {
  public:
    virtual void Init(class Main* master);
    virtual void Destroy();
    virtual void Frame(float deltaTime, class Viewport* viewport);

    private:
    void UpdateCameraPosition();

    private:
#ifdef __EMSCRIPTEN__
    SceneRenderer m_sceneRenderer;
#endif

    private:
    Entity* m_playerPrefab = nullptr;
    Entity* m_mainCam = nullptr;
    Entity* m_floor = nullptr;
  };
}

extern "C" TK_GAME_API ToolKit::Game * TK_STDCAL CreateInstance();