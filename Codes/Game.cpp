#include "Game.h"
#include "DirectionComponent.h"
#include "GameUtils.h"

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

    // ProjectileManager
    m_projectileManager = new ProjectileManager(m_mainScene);

    // Input manager
    m_inputManager = new InputManager();

    // Player prefab
    Entity* playerPrefab = GetSceneManager()->GetCurrentScene()->GetFirstByTag("playerPrefab");
    playerPrefab->AddComponent(std::make_shared<DirectionComponent>()); // Add direction component

    // Projectile start pos
    Vec3 projectileStartPos = ZERO;
    for (Node* child : playerPrefab->m_node->m_children[0]->m_children)
    {
      if (child->m_entity->GetTagVal() == "projectileStartPos")
      {
        projectileStartPos = child->GetTranslation();
        break;
      }
    }

    // Player controller
    m_playerController = new PlayerController(playerPrefab, m_inputManager, m_projectileManager);
    m_playerController->m_projectileStartPos = projectileStartPos;
    m_playerController->m_scene = m_mainScene;
    m_playerController->Init();

    // Attach the camera to the player prefab
    m_mainCam = GetSceneManager()->GetCurrentScene()->GetFirstByTag("mainCam");
    playerPrefab->m_node->AddChild(m_mainCam->m_node, true);
  }

  void Game::Destroy()
  {
    // Remove direction component from player
    m_playerController->m_playerPrefab->RemoveComponent(m_playerController->m_playerPrefab->GetComponent<DirectionComponent>()->m_id);

    SafeDel(m_playerController);
    SafeDel(m_inputManager);
    SafeDel(m_projectileManager);

    delete this;
  }

  void Game::Frame(float deltaTime, class Viewport* viewport)
  {
    GameUtils::m_viewport = viewport;

    viewport->AttachCamera(m_mainCam->GetIdVal());

    m_inputManager->Update();
    m_projectileManager->UpdateProjectiles(deltaTime);

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