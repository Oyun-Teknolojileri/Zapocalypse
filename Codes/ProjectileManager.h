#pragma once
#include "ToolKit.h"

namespace ToolKit
{
  class Projectile
  {
    // Prevent creating projectile that is not tracked by the manager
    friend class ProjectileManager;

    private:
    Projectile();
    ~Projectile();

    private:
    Entity* entity;
    Vec3 direction; // Should be a unit vector
    float speed;
    std::function<void(Entity* projectile, Entity* hit)> callback;
    short nextAvailableProjectileIndex;
    bool active;
  };

  class ProjectileManager
  {
    public:
    ProjectileManager(ScenePtr scene);
    ~ProjectileManager();

    // Returns true if the projectile can be shot
    // Note: Direction should be a unit vector
    // Note: Entity must have a mesh component (so the collision check can be done)
    bool ShootProjectile(const Vec3& pos, const Vec3& dir, float speed, std::function<void(Entity* projectile, Entity* hit)> callback);
    void UpdateProjectiles(float deltaTime);
    void Reset(ScenePtr newScene);
    void AddToProjectileIgnoreList(ULongID id);
    void RemoveFromProjectileIgnoreList(ULongID id);

    private:
    ProjectileManager() {};
    void RemoveProjectile(unsigned index);

    private:
    ScenePtr m_scene = nullptr;

    // A memory pool for projectile objects
    static constexpr unsigned MAX_PROJECTILES = 128;
    Projectile m_projectilePool[MAX_PROJECTILES];

    // For indices, -1 means end of the list
    int m_availableProjectileIndex = 0;

    std::vector<ULongID> m_projectileHitIgnoreList;
  };
}
