#ifndef IMGUI_SCOREBOARD_H
#define IMGUI_SCOREBOARD_H

#include "imgui_window.h"
#include "hud.h"
#include "cl_util.h"

class CImGuiScoreboard : public IImGuiWindow
{
public:
    void Initialize();

    void InitHUDData();

    void VidInitialize();
    void Terminate();
    void Think();
    void Draw();

    bool Active();
    bool CursorRequired();
    bool HandleKey(bool keyDown, int keyNumber, const char* bindName);

    void DeathMsg(int killer, int victim);

    int MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf);
    int MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf);
    int MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf);

    void UserCmd_ShowScores();
    void UserCmd_HideScores();

    void DrawScoreboard();
    void DrawTeams();
    void DrawPlayers(const char *team);
    void GetAllPlayersInfo();
    bool ShouldDrawScoreboard() const;

    int m_iPlayerNum;
    int m_iNumTeams;
    bool m_bShowscoresHeld = false;

private:
    int m_iLastKilledBy;
    float m_fLastKillTime;
    float m_fLineStartX;
    float m_fLineEndX;
};

extern CImGuiScoreboard m_iScoreboard;

#endif // IMGUI_SCOREBOARD_H
