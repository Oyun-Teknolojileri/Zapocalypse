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
      return m_viewport->RayFromMousePosition();
    }

    private:
    inline static GameUtils::Viewport* m_viewport = nullptr;
  };
}