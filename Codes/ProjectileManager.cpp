#include "ProjectileManager.h"

#include <Primative.h>
#include <MeshComponent.h>
#include <Material.h>
#include <Mesh.h>
#include <Scene.h>
#include <MathUtil.h>

namespace ToolKit
{
  Projectile::Projectile()
  {
    // Projectiles have a default mesh and material
    CubePtr cube = MakeNewPtr<Cube>();
    cube->SetCubeScaleVal(Vec3(0.1f));
    entity = cube;
    
    static unsigned nameIndex = 0;
    nameIndex++;
    entity->SetNameVal("Projectile " + std::to_string(nameIndex));
    entity->SetTagVal("pr"); // This tag string is for projectiles only, do not use it elsewhere!
    MaterialPtr mat = GetMaterialManager()->GetCopyOfUnlitColorMaterial();
    mat->m_color = Vec3(1.0f, 1.0f, 0.3f);
    mat->m_emissiveColor = Vec3(1.0f, 1.0f, 0.3f);
    entity->GetComponent<MeshComponent>()->GetMeshVal()->SetMaterial(mat);
    
    lifeTime = 1000.0f;
    duration = 0.0f;
  }

  Projectile::~Projectile()
  {
    entity->ClearComponents();
    entity = nullptr;
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

  bool ProjectileManager::ShootProjectile(const Vec3& pos, const Vec3& dir, float speed, Projectile::HitCheckFn callback)
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
    projectile.duration = 0.0f;

    // Add projectile to the scene
    m_scene->AddEntity(projectile.entity);

    return true;
  }

  void ProjectileManager::UpdateProjectiles(float deltaTime)
  {
    auto IsInArrayFn = [](const EntityIdArray& list, EntityPtr obj)
    {
      for (ULongID id : list)
        if (id == obj->GetIdVal())
          return true;
      return false;
    };

    // Move projectiles
    for (unsigned i = 0; i < MAX_PROJECTILES; ++i)
    {
      Projectile& projectile = m_projectilePool[i];
      if (!projectile.active)
      {
        continue;
      }
      if (projectile.lifeTime < projectile.duration)
      {
        // Remove the projectile
        RemoveProjectile(i);
        continue;
      }
      else
      {
        projectile.duration += deltaTime;
      }

      BoundingBox projectileBB = projectile.entity->GetAABB(true);
  
      projectile.entity->m_node->Translate(projectile.direction * projectile.speed * deltaTime);

      StringArray ignoredTags =
      {
        "ignore-me",
        "mainCam",
        "light",
        "pr"
      };

      for (EntityPtr object : m_scene->GetEntities())
      {
        // Ignored entities
        if (object->GetMeshComponent() == nullptr) 
        {
          continue;
        }
        else
        {
          bool skip = false;
          for (size_t i = 0; i < ignoredTags.size(); i++)
          {
            if (object->GetTagVal() == ignoredTags[i])
            {
              skip = true;
              break;
            }
          }

          if (skip)
          {
            continue;
          }
        }

        if (BoxBoxIntersection(object->GetAABB(true), projectileBB))
        {
          // Call calback when hit an object
          if (projectile.callback != nullptr)
            projectile.callback(projectile.entity, object);

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
    Projectile& projectile = m_projectilePool[index];
    projectile.active = false;
    projectile.nextAvailableProjectileIndex = m_availableProjectileIndex;
    m_availableProjectileIndex = index;

    // Remove projectile from scene
    m_scene->RemoveEntity(m_projectilePool[index].entity->GetIdVal());
  }
}