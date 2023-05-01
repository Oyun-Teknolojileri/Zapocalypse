#pragma once
#include "ToolKit.h"

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

    private:
    inline static Viewport* GameViewport = nullptr;
    inline static float SceneFloorY = 0.0f;
  };
}