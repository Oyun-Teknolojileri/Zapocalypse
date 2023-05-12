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

    // Floor entity
    m_floor = m_mainScene->GetFirstByTag("floor");

    // Player prefab
    m_playerPrefab = m_mainScene->GetFirstByTag("playerPrefab");
    m_playerPrefab->AddComponent(std::make_shared<DirectionComponent>()); // Add direction component
    // Snap player to floor
    Vec3 playerPos = m_playerPrefab->m_node->GetTranslation();
    float halfHeight = m_playerPrefab->m_node->m_children[0]->m_entity->GetMeshComponent()->GetAABB().GetHeight() / 2.0f;
    m_playerPrefab->m_node->SetTranslation(Vec3(playerPos.x, m_floor->m_node->GetTranslation().y + halfHeight, playerPos.z));

    // Projectile start pos
    Vec3 projectileStartPos = ZERO;
    for (Node* child : m_playerPrefab->m_node->m_children[0]->m_children)
    {
      if (child->m_entity->GetTagVal() == "projectileStartPos")
      {
        projectileStartPos = child->GetTranslation();
        break;
      }
    }

    // Player controller
    m_playerController = new PlayerController(m_playerPrefab, m_inputManager, m_projectileManager);
    m_playerController->m_scene = m_mainScene;
    m_playerController->Init();

    // Attach the camera to the player prefab
    m_mainCam = GetSceneManager()->GetCurrentScene()->GetFirstByTag("mainCam");
    UpdateCameraPosition();
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
    viewport->AttachCamera(m_mainCam->GetIdVal());

    GameUtils::GameViewport = viewport;
    GameUtils::SceneFloorY = m_floor->m_node->GetTranslation().y;

    m_inputManager->Update();
    m_projectileManager->UpdateProjectiles(deltaTime);
    m_playerController->Update(deltaTime);
    UpdateCameraPosition();

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

  void Game::UpdateCameraPosition()
  {
    static const Vec3 cameraDistance = Vec3(5.0f, 5.0f, 5.0f);
    const Vec3 playerPos = m_playerPrefab->m_node->GetTranslation();
    const Vec3 targetPos = playerPos + cameraDistance;
    const Vec3 delta = targetPos - m_mainCam->m_node->GetTranslation();
    if (GameUtils::SquaredLength(delta) < 0.02f)
    {
       m_mainCam->m_node->SetTranslation(targetPos);
    }
    else
    {
      m_mainCam->m_node->Translate(delta * 0.1f);
    }
  }
}
