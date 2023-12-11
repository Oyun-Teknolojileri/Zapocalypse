#include "Game.h"
#include "GameUtils.h"
#include "GameGlobals.h"

#include <DirectionComponent.h>
#include <Scene.h>
#include <Viewport.h>

ToolKit::Game Self;
extern "C" TK_PLUGIN_API ToolKit::Game* TK_STDCAL GetInstance() { return &Self; }

namespace ToolKit
{
  void Game::OnGameStart()
  {
    // Scene
    g_gameGlobals.m_currentScene = GetSceneManager()->Create<Scene>(ScenePath("MainScene.scene"));
    GetSceneManager()->SetCurrentScene(g_gameGlobals.m_currentScene);

    // ProjectileManager
    g_gameGlobals.m_projectileManager = MakeNewPtr<ProjectileManager>(g_gameGlobals.m_currentScene);

    // Input manager
    g_gameGlobals.m_inputManager = MakeNewPtr<InputManager>();

    // Floor entity
    m_floor = g_gameGlobals.m_currentScene->GetFirstByTag("floor");
    if (!m_floor)
    {
      m_currentState = PluginState::Stop;
      return;
    }

    // Floor border coordinates
    // Note: If the floor size is changable, update this variable every frame
    GameUtils::FloorBoundingBox = m_floor->GetAABB(true);

    // Player prefab
    m_playerPrefab = g_gameGlobals.m_currentScene->GetFirstByTag("playerPrefab");
    if (!m_playerPrefab)
    {
      m_currentState = PluginState::Stop;
      return;
    }

    m_playerPrefab->AddComponent<DirectionComponent>(); // Add direction component
    // Snap player to floor
    Vec3 playerPos = m_playerPrefab->m_node->GetTranslation();
    float halfHeight = m_playerPrefab->m_node->m_children[0]->OwnerEntity()->GetMeshComponent()->GetAABB().GetHeight() / 2.0f;
    m_playerPrefab->m_node->SetTranslation(Vec3(playerPos.x, m_floor->m_node->GetTranslation().y + halfHeight, playerPos.z));

    // Note: If the player size is changable, update this variable every frame
    GameUtils::HalfPlayerHeight = m_playerPrefab->m_node->m_children[0]->OwnerEntity()->GetMeshComponent()->GetAABB().GetHeight() / 2.0f;

    // Projectile start pos
    Vec3 projectileStartPos = ZERO;
    for (Node* child : m_playerPrefab->m_node->m_children[0]->m_children)
    {
      if (child->OwnerEntity()->GetTagVal() == "projectileStartPos")
      {
        projectileStartPos = child->GetTranslation();
        break;
      }
    }

    // Enemy controller
    GameUtils::EnemyPrefabPath = "EnemyPrefab.scene";
    g_gameGlobals.m_enemyController = MakeNewPtr<EnemyController>();
    for (EntityPtr enemyPrefabEntity : g_gameGlobals.m_currentScene->GetByTag("enemyPrefab"))
    {
      g_gameGlobals.m_enemyController->AddEnemy(enemyPrefabEntity);
    }
    g_gameGlobals.m_enemyController->Init(g_gameGlobals.m_currentScene);

    // Player controller
    g_gameGlobals.m_playerController = MakeNewPtr<PlayerController>(m_playerPrefab);
    g_gameGlobals.m_playerController->m_scene = g_gameGlobals.m_currentScene;
    g_gameGlobals.m_playerController->Init();

    // Attach the camera to the player prefab
    m_mainCam = GetSceneManager()->GetCurrentScene()->GetFirstByTag("mainCam");
    if (!m_mainCam)
    {
      m_currentState = PluginState::Stop;
      return;
    }
    UpdateCameraPosition();

    // Get UI Layer & dpad
    m_uiLayer = std::make_shared<UILayer>(LayerPath("TestButton.layer"));
    m_uiLayerAddedToViewport = false;
    bool dpadFound = false;
    for (EntityPtr ntt : m_uiLayer->m_scene->GetEntities())
    {
      if (ntt->IsA<Dpad>())
      {
        g_gameGlobals.m_inputManager->Init(Cast<Dpad>(ntt));
        dpadFound = true;
        break;
      }
    }

    if (!dpadFound)
    {
      m_currentState = PluginState::Stop;
      GetLogger()->WriteTKConsole(LogType::Error, "No Dpad found in UI layer!");
    }
  }

  void Game::Destroy()
  {
    if (GameUtils::GameViewport != nullptr && m_uiLayer != nullptr)
    {
      GetUIManager()->RemoveLayer(GameUtils::GameViewport->m_viewportId, m_uiLayer->m_id);
    }

    // Remove direction component from player
    if (g_gameGlobals.m_playerController != nullptr)
    {
        if (g_gameGlobals.m_playerController->m_playerPrefab != nullptr)
        {
            g_gameGlobals.m_playerController->m_playerPrefab->RemoveComponent(g_gameGlobals.m_playerController->m_playerPrefab->GetComponent<DirectionComponent>()->GetIdVal());
        }
    }

    g_gameGlobals.m_playerController = nullptr;
    g_gameGlobals.m_inputManager = nullptr;
    g_gameGlobals.m_projectileManager = nullptr;
    g_gameGlobals.m_enemyController = nullptr;

    if (g_gameGlobals.m_currentScene != nullptr)
    {
        //g_gameGlobals.m_currentScene->Destroy(false);
    }
  }

  void Game::Frame(float deltaTime)
  {
    if (m_firstFrame)
    {
      OnGameStart();

      m_firstFrame = false;
    }

    m_viewport->AttachCamera(m_mainCam->GetIdVal());
    bool test = Cast<Camera>(m_mainCam)->IsOrtographic();

    if (!m_uiLayerAddedToViewport)
    {
      m_uiLayerAddedToViewport = true;
      GetUIManager()->AddLayer(m_viewport->m_viewportId, m_uiLayer);
    }

    GameUtils::GameViewport = m_viewport;
    GameUtils::SceneFloorY = m_floor->m_node->GetTranslation().y;

    g_gameGlobals.m_inputManager->Update();
    g_gameGlobals.m_projectileManager->UpdateProjectiles(deltaTime);
    g_gameGlobals.m_playerController->Update(deltaTime);
    g_gameGlobals.m_enemyController->Update(deltaTime);
    UpdateCameraPosition();
  }

  void Game::UpdateCameraPosition()
  {
    static const Vec3 cameraDistance = Vec3(5.0f, 5.0f, 5.0f) * 2.0f;
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
