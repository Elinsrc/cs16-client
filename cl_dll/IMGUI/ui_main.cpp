#include "imgui_window_system.h"
#include "ui_demo_window.h"
#include "ui_ScorePanel.h"

void CImGuiWindowSystem::LinkWindows()
{
    static CImGuiDemoWindow demoWindow;
    AddWindow(&demoWindow);

    AddWindow(&m_iScoreboard);
}
