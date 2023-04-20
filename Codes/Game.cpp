#include "Game.h"

extern "C" TK_GAME_API ToolKit::Game * TK_STDCAL CreateInstance()
{
  return new ToolKit::Game();
}

namespace ToolKit
{

  void Game::Init(Main* master)
  {
    Main::SetProxy(master);

    // Scene
    m_mainScene = GetSceneManager()->Create<Scene>(ScenePath("MainScene.scene"));
    GetSceneManager()->SetCurrentScene(m_mainScene);

    // Camera attached to the player
    m_mainCam = GetSceneManager()->GetCurrentScene()->GetFirstByTag("mainCam");

    // Player and player controller
    Entity* player = GetSceneManager()->GetCurrentScene()->GetFirstByTag("player");
    m_playerController = new PlayerController(player);
  }

  void Game::Destroy()
  {
    SafeDel(m_playerController);

    delete this;
  }

  void Game::Frame(float deltaTime, class Viewport* viewport)
  {
    viewport->AttachCamera(m_mainCam->GetIdVal());

    // Update player controller
    m_playerController->Update(deltaTime);

#ifdef __EMSCRIPTEN__
    GetRenderSystem()->ExecuteRenderTasks();

    m_sceneRenderer.m_params.Cam = camera;
    m_sceneRenderer.m_params.ClearFramebuffer = true;
    m_sceneRenderer.m_params.Gfx.BloomIntensity = 0.0;
    m_sceneRenderer.m_params.Lights = GetSceneManager()->GetCurrentScene()->GetLights();
    m_sceneRenderer.m_params.MainFramebuffer = viewport->m_framebuffer;
    m_sceneRenderer.m_params.Scene = GetSceneManager()->GetCurrentScene();
    GetRenderSystem()->AddRenderTask(&m_sceneRenderer);

    static uint totalFrameCount = 0;
    GetRenderSystem()->SetFrameCount(totalFrameCount++);
#endif
  }

}