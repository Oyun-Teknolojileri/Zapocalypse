#pragma once

#include "PlayerController.h"
#include "InputManager.h"
#include "ProjectileManager.h"
#include "EnemyController.h"

#include <ToolKit.h>
#include <Plugin.h>
#include <SceneRenderPath.h>   

namespace ToolKit
{
  class  Game : public GamePlugin
  {
  public:
    virtual void Init(class Main* master);
    virtual void Destroy();
    virtual void Frame(float deltaTime, class Viewport* viewport);

    private:
    void UpdateCameraPosition();

    private:
    EntityPtr m_playerPrefab = nullptr;
    EntityPtr m_mainCam = nullptr;
    EntityPtr m_floor = nullptr;
  };
}

extern "C" TK_GAME_API ToolKit::Game* TK_STDCAL CreateInstance();