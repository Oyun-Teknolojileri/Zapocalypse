#pragma once

#include "PlayerController.h"
#include "InputManager.h"
#include "ProjectileManager.h"
#include "EnemyController.h"

#include <ToolKit.h>
#include <Plugin.h>
#include <UIManager.h>

namespace ToolKit
{
  class Game : public GamePlugin
  {
  public:
    Game() {}
    virtual ~Game() {}
    virtual void Init(class Main* master);
    virtual void Destroy();
    void Frame(float deltaTime, class Viewport* viewport) override;

    private:
    virtual void UpdateCameraPosition();

    private:
    EntityPtr m_playerPrefab = nullptr;
    EntityPtr m_mainCam = nullptr;
    EntityPtr m_floor = nullptr;

    UILayerPtr m_uiLayer = nullptr;
    bool m_uiLayerAddedToViewport = false;
    class Viewport* m_viewport = nullptr;
  };
}

extern "C" TK_GAME_API ToolKit::Game* TK_STDCAL CreateInstance();