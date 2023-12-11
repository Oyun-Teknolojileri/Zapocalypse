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
    virtual void Init(class Main* master) { Main::SetProxy(master); }
    void OnGameStart();
    virtual void Destroy();
    void Frame(float deltaTime) override;

    void OnLoad(XmlDocumentPtr state) override {}
    void OnUnload(XmlDocumentPtr state) override {}

    private:
    virtual void UpdateCameraPosition();

    private:
    EntityPtr m_playerPrefab = nullptr;
    EntityPtr m_mainCam = nullptr;
    EntityPtr m_floor = nullptr;

    UILayerPtr m_uiLayer = nullptr;
    bool m_uiLayerAddedToViewport = false;

    bool m_firstFrame = true;
  };
}

extern "C" TK_PLUGIN_API ToolKit::Game* TK_STDCAL GetInstance();