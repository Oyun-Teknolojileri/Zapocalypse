#pragma once

#include "ToolKit.h"
#include "Plugin.h"
#include "SceneRenderer.h"
#include "PlayerController.h"

namespace ToolKit
{

  class Game : public GamePlugin
  {
  public:
    virtual void Init(class Main* master);
    virtual void Destroy();
    virtual void Frame(float deltaTime, class Viewport* viewport);

    private:
#ifdef __EMSCRIPTEN__
    SceneRenderer m_sceneRenderer;
#endif

    ScenePtr m_mainScene = nullptr;
    Entity* m_mainCam = nullptr;
    PlayerController* m_playerController = nullptr;
  };

}

extern "C" TK_GAME_API ToolKit::Game * TK_STDCAL CreateInstance();