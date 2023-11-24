#include "InputManager.h"
#include <Events.h>

namespace ToolKit
{
  void InputManager::Init(DpadPtr dpad)
  {
    m_dpad = dpad;
  }

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
      else if (event->m_action == EventAction::LeftClick)
      {
        MouseEvent *me = static_cast<MouseEvent *>(event);
        if (!me->m_release)
        {
          m_leftMouseDown = true;
        }
        else
        {
          m_leftMouseDown = false;
        }
      }
    }
  }
}