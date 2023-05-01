#pragma once

#include "ToolKit.h"
#include "Plugin.h"
#include "SceneRenderer.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "ProjectileManager.h"

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

    ScenePtr m_mainScene = nullptr;
    Entity* m_playerPrefab = nullptr;
    Entity* m_mainCam = nullptr;
    Entity* m_floor = nullptr;

    InputManager* m_inputManager = nullptr;
    PlayerController* m_playerController = nullptr;
    ProjectileManager* m_projectileManager = nullptr;
  };
}

extern "C" TK_GAME_API ToolKit::Game * TK_STDCAL CreateInstance();