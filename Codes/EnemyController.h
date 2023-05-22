#pragma once

#include <unordered_map>
#include "Toolkit.h"

namespace ToolKit
{
  class Enemy;

  class EnemyBaseState : public State
  {
   public:
    EnemyBaseState(Enemy* enemy) : m_enemy(enemy) {};
    virtual ~EnemyBaseState() {}
    void TransitionIn(State* prevState) override {}
    void TransitionOut(State* nextState) override {}

   private:
    EnemyBaseState() {}
    
   protected:
    Enemy* m_enemy = nullptr;
  };

  // Movement State Machine
  //////////////////////////////////////////////////////////////////////////

  class EnemyMovementSignal
  {
   public:
    static SignalId Move = 1;
    static SignalId Stop = 2;
  };

  class EnemyMovementState
  {
   public:
    static String Null;
    static String Stationary;
    static String Walk;
  };

  class EnemyStationaryState : public EnemyBaseState
  {
   public:
    EnemyStationaryState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyStationaryState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyMovementState::Stationary; }
  };

  class EnemyWalkState : public EnemyBaseState
  {
   public:
    EnemyWalkState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyWalkState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyMovementState::Walk; }
  };

  //////////////////////////////////////////////////////////////////////////

  class Enemy
  {
    friend class EnemyController;
    friend EnemyBaseState;
    friend EnemyStationaryState;
    friend EnemyWalkState;

    public:
    Enemy(Entity* entity, class EnemyController* controller);
    ~Enemy();

    void Update(float deltaTime);
    void GetHit(float damageAmount);
    void Die();

    void SetMovementTarget(const Vec3 targetPos);

    private:
    Enemy() {}

    private:
    Entity* m_enemyPrefab = nullptr;
    float m_health;

    EnemyController* m_enemyController = nullptr;

    StateMachine m_movementSM;
    // TODO StateMachine m_decisionSM; // Combat SM can be seperable, but since it is very simple it is being handled here

    Vec3 m_movementTargetPos;
    Vec3 m_movementTargetDir;
  };

  class EnemyController
  {
    public:
    ~EnemyController();

    void Init();
    void AddEnemy(Entity* entity);
    void Update(float deltaTime);
    void HitEnemy(ULongID id, float damageAmount);
    void KillEnemy(ULongID id);

    public:
    float m_enemyWalkSpeed = 0.001f;

    private:
    std::unordered_map<ULongID, Enemy*> m_enemies;

  };
};
