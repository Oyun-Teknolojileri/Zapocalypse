#pragma once

#include <Toolkit.h>
#include <StateMachine.h>
#include <Entity.h>

#include <unordered_map>

namespace ToolKit
{
  class Enemy;

  class  EnemyBaseState : public State
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

  class  EnemyMovementSignal
  {
   public:
    static SignalId Move = 1;
    static SignalId Stop = 2;
  };

  class  EnemyMovementState
  {
   public:
    static String Null;
    static String Stationary;
    static String Walk;
  };

  class  EnemyStationaryState : public EnemyBaseState
  {
   public:
    EnemyStationaryState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyStationaryState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyMovementState::Stationary; }
  };

  class  EnemyWalkState : public EnemyBaseState
  {
   public:
    EnemyWalkState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyWalkState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyMovementState::Walk; }
    void TransitionIn(State* prevState) override;
    void TransitionOut(State* nextState) override;
  };

  // Decision State Machine
  //////////////////////////////////////////////////////////////////////////

  class  EnemyDecisionSignal
  {
   public:
    static SignalId Patrol = 1;
    static SignalId Attack = 2;
  };

  class  EnemyDecisionState
  {
   public:
    static String Null;
    static String Patrol;
    static String Attack;
  };

  class  EnemyPatrolState : public EnemyBaseState
  {
   public:
    EnemyPatrolState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyPatrolState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyDecisionState::Patrol; }
    void TransitionIn(State* prevState) override;

    void SetNextTarget();
  };

  class  EnemyAttackState : public EnemyBaseState
  {
   public:
    EnemyAttackState(Enemy* enemy) : EnemyBaseState(enemy) {}
    virtual ~EnemyAttackState() {}

    SignalId Update(float deltaTime) override;
    String Signaled(SignalId signal) override;
    String GetType() override { return EnemyDecisionState::Attack; }
  };

  //////////////////////////////////////////////////////////////////////////

  class  Enemy
  {
    friend class EnemyController;
    friend EnemyBaseState;
    friend EnemyStationaryState;
    friend EnemyWalkState;
    friend EnemyPatrolState;
    friend EnemyAttackState;

    public:
    Enemy(EntityPtr entity, class EnemyController* controller);
    ~Enemy();

    void Update(float deltaTime);
    void GetHit(float damageAmount);
    void Die();

    void SetMovementTarget(const Vec3 targetPos);
    inline Vec3 GetProjectileStartPos()
    {
      static Vec3 muzzlePos = ZERO;
      static bool found = false;
      if (!found)
      {
        for (Node* childNode : m_enemyPrefab->m_node->m_children[0]->m_children)
        {
          if (childNode->m_entity->GetTagVal() == "enemyMuzzlePos")
          {
            muzzlePos = childNode->GetTranslation();
            break;
          }
        }
      }
      return muzzlePos;
    }

    private:
    Enemy() {}

    private:
    EntityPtr m_enemyPrefab = nullptr;
    float m_health;

    EnemyController* m_enemyController = nullptr;

    StateMachine m_movementSM;
    StateMachine m_decisionSM; // Combat SM can be seperable, but since it is very simple it is being handled here

    Vec3 m_movementTargetPos;
    Vec3 m_movementTargetDir;
    bool m_destionationReached;
    
    unsigned int m_lastPatrolIndex = 0;
  };

  class  EnemyController
  {
    public:
    ~EnemyController();

    void Init(const ScenePtr scene);
    void AddEnemy(EntityPtr entity);
    void Update(float deltaTime);
    void HitEnemy(ULongID id, float damageAmount);
    void KillEnemy(ULongID id);

    inline Vec3Array GetPatrolPoints() const { return m_patrolPoints; }

    private:
    void FillEnemyStates(Enemy* enemy);
    void SpawnEnemy();

    private:
    std::unordered_map<ULongID, Enemy*> m_enemies;
    Vec3Array m_patrolPoints;
  };
};
