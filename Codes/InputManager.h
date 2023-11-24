#pragma once

#include <ToolKit.h>
#include <Types.h>
#include <Dpad.h>

namespace ToolKit
{
  class  InputManager
  {
    public:

    void Init(DpadPtr dpad);
    void Update();

    inline bool WDown()
    {
      return m_wDown;
    }
    inline bool SDown()
    {
      return m_sDown;
    }
    inline bool ADown()
    {
      return m_aDown;
    }
    inline bool DDown()
    {
      return m_dDown;
    }
    inline bool LeftMouseDown()
    {
      return m_leftMouseDown;
    }
    inline float DpadRadius()
    {
      return m_dpad->GetRadius();
    }
    inline float DpadX()
    {
      return m_dpad->GetDeltaX();
    }
    inline float DpadY()
    {
      return m_dpad->GetDeltaY();
    }

    private:
    bool m_wDown = false;
    bool m_sDown = false;
    bool m_aDown = false;
    bool m_dDown = false;
    bool m_leftMouseDown = false;
    DpadPtr m_dpad = nullptr;  
  };
}