#include "PlayerController.h"

namespace ToolKit
{
  void PlayerController::Update(float deltaTime)
  {
    // Get Inputs
    EventPool& events = Main::GetInstance()->m_eventPool;
    for (Event* event : events)
    {
      if (event->m_type == Event::EventType::Keyboard)
      {
        KeyboardEvent* ke = static_cast<KeyboardEvent*>(event);
        if (ke->m_keyCode == 'w')
        {
          GetLogger()->WriteConsole(LogType::Command, "test");
        }
      }
    }
  }
}