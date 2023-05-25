#include "ProjectileManager.h"
#include "MeshComponent.h"

namespace ToolKit
{
  Projectile::Projectile()
  {
    // Projectiles have a default mesh and material
    entity = new Cube(Vec3(0.1f));
    static unsigned nameIndex = 0;
    nameIndex++;
    entity->SetNameVal("Projectile " + std::to_string(nameIndex));
    entity->SetTagVal("pr"); // This tag string is for projectiles only, do not use it elsewhere!
    MaterialPtr mat = GetMaterialManager()->GetCopyOfUnlitColorMaterial();
    mat->m_color = Vec3(1.0f, 1.0f, 0.3f);
    mat->m_emissiveColor = Vec3(1.0f, 1.0f, 0.3f);
    entity->GetComponent<MeshComponent>()->GetMeshVal()->SetMaterial(mat);
    
    lifeTime = 5000.0f;
    duration = 0.0f;
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

  bool ProjectileManager::ShootProjectile(const Vec3& pos, const Vec3& dir, float speed, std::function<void(Entity* projectile, Entity* hit)> callback)
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
    auto IsInArrayFn = [](const EntityIdArray& list, Entity* obj)
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

      // Tag anything that will be ignored.
      // You can concatenate tags from the editor with "."
      // EX: tag = "first.second.ignore"
      // when you query an object with tag "first", "second" or "ignore" , it will ignore the entity with the above tag.
      EntityRawPtrArray projectileIgnored;
      for (const String& tag : ignoredTags)
      {
        EntityRawPtrArray concatList = m_scene->GetByTag(tag);
        projectileIgnored.insert(projectileIgnored.end(), concatList.begin(), concatList.end());
      }
      
      EntityIdArray ignoredHandles;
      ToEntityIdArray(ignoredHandles, projectileIgnored);

      ignoredHandles.insert(ignoredHandles.end(), m_projectileHitIgnoreList.begin(), m_projectileHitIgnoreList.end());

      // Collision check with the environment in the scene
      for (Entity* object : m_scene->GetEntities())
      {
        // Ignored entities
        if (object->GetMeshComponent() == nullptr || IsInArrayFn(ignoredHandles, object)) 
        {
          continue;
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