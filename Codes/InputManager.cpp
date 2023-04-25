#include "InputManager.h"

namespace ToolKit
{
  void InputManager::Update()
  {
    EventPool& events = Main::GetInstance()->m_eventPool;
    for (Event* event : events)
    {
      if (event->m_type == Event::EventType::Keyboard)
      {
        KeyboardEvent* ke = static_cast<KeyboardEvent*>(event);

        if (ke->m_action == EventAction::KeyDown)
        {
          if (ke->m_keyCode == 'w')
          {
            m_wDown = true;
          }
          else if (ke->m_keyCode == 's')
          {
            m_sDown = true;
          }
          else if (ke->m_keyCode == 'a')
          {
            m_aDown = true;
          }
          else if (ke->m_keyCode == 'd')
          {
            m_dDown = true;
          }
        }

        if (ke->m_action == EventAction::KeyUp)
        {
          if (ke->m_keyCode == 'w')
          {
            m_wDown = false;
          }
          else if (ke->m_keyCode == 's')
          {
            m_sDown = false;
          }
          else if (ke->m_keyCode == 'a')
          {
            m_aDown = false;
          }
          else if (ke->m_keyCode == 'd')
          {
            m_dDown = false;
          }
        }
      }
    }
  }
}