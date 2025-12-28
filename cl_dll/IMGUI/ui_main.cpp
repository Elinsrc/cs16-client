#include "imgui_window_system.h"
#include "ui_demo_window.h"
#include "ui_ScorePanel.h"
#include "ui_MOTD.h"

void CImGuiWindowSystem::LinkWindows()
{
    static CImGuiDemoWindow demoWindow;
    AddWindow(&demoWindow);

    AddWindow(&m_iScoreboard);

    AddWindow(&m_iMOTD);
}
