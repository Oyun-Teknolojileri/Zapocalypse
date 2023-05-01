#include "ProjectileManager.h"
#include "MeshComponent.h"

namespace ToolKit
{
  Projectile::Projectile()
  {
    entity = new Cube(0.1f);
    // Projectiles have a default mesh and material
    // TODO add mesh
    // TODO Add material
  }

  Projectile::~Projectile()
  {
    entity->ClearComponents();
    delete entity;
  }

  ProjectileManager::ProjectileManager(ScenePtr scene)
  {
    Reset(scene);
  }

  ProjectileManager::~ProjectileManager()
  {
    for (Projectile& projectile : m_projectilePool)
    {
      if (projectile.active)
        m_scene->RemoveEntity(projectile.entity->GetIdVal());
    }
  }

  bool ProjectileManager::ShootProjectile(const Vec3& pos, const Vec3& dir, float speed, std::function<void(Entity* hit)> callback)
  {
    if (m_availableProjectileIndex == -1) // Check if the pool is full
    {
      return false;
    }

    // Create a new projectile
    int newProjectileIndex = m_availableProjectileIndex;
    Projectile& projectile = m_projectilePool[newProjectileIndex];
    m_availableProjectileIndex = projectile.nextAvailableProjectileIndex;
    projectile.entity->m_node->SetTranslation(pos);
    projectile.direction = dir;
    projectile.speed = speed;
    projectile.callback = callback;
    projectile.active = true;

    // Add projectile to the scene
    m_scene->AddEntity(projectile.entity);

    return true;
  }

  void ProjectileManager::UpdateProjectiles(float deltaTime)
  {
    auto IsIdInListFn = [](const std::vector<ULongID>& list, ULongID id)
    {
      for (ULongID listId : list)
        if (listId == id)
          return true;
      return false;
    };

    // Move projectiles
    for (unsigned i = 0; i < MAX_PROJECTILES; ++i)
    {
      Projectile& projectile = m_projectilePool[i];
      BoundingBox projectileBB = projectile.entity->GetAABB();
  
      projectile.entity->m_node->Translate(projectile.direction * projectile.speed * deltaTime);

      // Collision check with the environment in the scene
      for (Entity* object : m_scene->GetEntities())
      {
        // Ignore list
        if (IsIdInListFn(m_projectileHitIgnoreList, object->GetIdVal()))
        {
          continue;
        }

        // TODO avoid projectile hit each other
        if (BoxBoxIntersection(object->GetAABB(true), projectileBB))
        {
          // Call calback when hit an object
          if (projectile.callback != nullptr)
            projectile.callback(object);

          // Remove the projectile
          RemoveProjectile(i);
          break;
        }
      }
    }
  }

  void ProjectileManager::Reset(ScenePtr newScene)
  {
    // Reset the scene and the projectile pool
    m_scene = newScene;

    for (unsigned i = 0; i < MAX_PROJECTILES; ++i)
    {
      m_projectilePool[i].nextAvailableProjectileIndex = i + 1;
      m_projectilePool[i].active = false;
    }
    m_projectilePool[MAX_PROJECTILES - 1].nextAvailableProjectileIndex = -1; // End of the list

  }

  void ProjectileManager::AddToProjectileIgnoreList(ULongID id)
  {
    m_projectileHitIgnoreList.push_back(id);
  }

  void ProjectileManager::RemoveFromProjectileIgnoreList(ULongID id)
  {
    for (auto itr = m_projectileHitIgnoreList.begin(); itr != m_projectileHitIgnoreList.end(); ++itr)
    {
      if (*itr == id)
      {
        m_projectileHitIgnoreList.erase(itr);
        break;
      }
    }
  }

  void ProjectileManager::RemoveProjectile(unsigned index)
  {
    m_projectilePool[index].active = false;
    // Remove projectile from scene
    m_scene->RemoveEntity(m_projectilePool[index].entity->GetIdVal());
  }
}