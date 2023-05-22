#pragma once
#include "ToolKit.h"
#include "MathUtil.h"

namespace ToolKit
{
  // Utility class for game, does not know anything about the game, just for helpers
  class GameUtils
  {
    friend class Game;

    public:
    inline static Ray GetRayFromMousePosition()
    {
      return GameViewport->RayFromMousePosition();
    }

    inline static float GetFloorY()
    {
      return SceneFloorY;
    }

    inline static Quaternion QuatLookAtRH(const Vec3& direction) 
    {
      Mat3 Result = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
      };
      Result[2] = -glm::normalize(direction);
      Result[0] = glm::normalize(glm::cross(Y_AXIS, Result[2]));
      Result[1] = glm::cross(Result[2], Result[0]);
      return glm::quat_cast(Result);
    }

    inline static float SquaredLength(const Vec3& vec)
    {
      return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    }

    // https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
    template <typename T>
    inline static int Sgn(T val)
    {
      return (T(0) < val) - (val < T(0));
    }

    private:
    inline static Viewport* GameViewport = nullptr;
    inline static float SceneFloorY = 0.0f;
  };
}