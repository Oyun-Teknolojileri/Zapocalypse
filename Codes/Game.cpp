#include "Game.h"
#include "GameUtils.h"
#include "GameGlobals.h"

#include <DirectionComponent.h>
#include <Scene.h>

namespace ToolKit
{

  extern "C" TK_GAME_API ToolKit::Game* TK_STDCAL CreateInstance() { return new ToolKit::Game(); }

  void Game::Init(Main* master)
  {
    Main::SetProxy(master);

    // Scene
    g_gameGlobals.m_currentScene = GetSceneManager()->Create<Scene>(ScenePath("MainScene.scene"));
    GetSceneManager()->SetCurrentScene(g_gameGlobals.m_currentScene);

    // ProjectileManager
    g_gameGlobals.m_projectileManager = new ProjectileManager(g_gameGlobals.m_currentScene);

    // Input manager
    g_gameGlobals.m_inputManager = new InputManager();

    // Floor entity
    m_floor = g_gameGlobals.m_currentScene->GetFirstByTag("floor");

    // Floor border coordinates
    // Note: If the floor size is changable, update this variable every frame
    GameUtils::FloorBoundingBox = m_floor->GetAABB(true);

    // Player prefab
    m_playerPrefab = g_gameGlobals.m_currentScene->GetFirstByTag("playerPrefab");
    m_playerPrefab->AddComponent<DirectionComponent>(); // Add direction component
    // Snap player to floor
    Vec3 playerPos = m_playerPrefab->m_node->GetTranslation();
    float halfHeight = m_playerPrefab->m_node->m_children[0]->m_entity->GetMeshComponent()->GetAABB().GetHeight() / 2.0f;
    m_playerPrefab->m_node->SetTranslation(Vec3(playerPos.x, m_floor->m_node->GetTranslation().y + halfHeight, playerPos.z));

    // Note: If the player size is changable, update this variable every frame
    GameUtils::HalfPlayerHeight = m_playerPrefab->m_node->m_children[0]->m_entity->GetMeshComponent()->GetAABB().GetHeight() / 2.0f;

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

    // Enemy controller
    GameUtils::EnemyPrefabPath = "EnemyPrefab.scene";
    g_gameGlobals.m_enemyController = new EnemyController();
    for (EntityPtr enemyPrefabEntity : g_gameGlobals.m_currentScene->GetByTag("enemyPrefab"))
    {
      g_gameGlobals.m_enemyController->AddEnemy(enemyPrefabEntity);
    }
    g_gameGlobals.m_enemyController->Init(g_gameGlobals.m_currentScene);

    // Player controller
    g_gameGlobals.m_playerController = MakeNew<PlayerController>(m_playerPrefab);
    g_gameGlobals.m_playerController->m_scene = g_gameGlobals.m_currentScene;
    g_gameGlobals.m_playerController->Init();

    // Attach the camera to the player prefab
    m_mainCam = GetSceneManager()->GetCurrentScene()->GetFirstByTag("mainCam");
    UpdateCameraPosition();
  }

  void Game::Destroy()
  {
    // Remove direction component from player
    g_gameGlobals.m_playerController->m_playerPrefab->RemoveComponent(g_gameGlobals.m_playerController->m_playerPrefab->GetComponent<DirectionComponent>()->GetIdVal());

    SafeDel(g_gameGlobals.m_playerController);
    SafeDel(g_gameGlobals.m_inputManager);
    SafeDel(g_gameGlobals.m_projectileManager);
    SafeDel(g_gameGlobals.m_enemyController);

    g_gameGlobals.m_currentScene->ClearEntities();

    delete this;
  }

  void Game::Frame(float deltaTime, class Viewport* viewport)
  {
    // TODOD debug
    ScenePtr s = g_gameGlobals.m_currentScene;

    viewport->AttachCamera(m_mainCam->GetIdVal());

    GameUtils::GameViewport = viewport;
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
