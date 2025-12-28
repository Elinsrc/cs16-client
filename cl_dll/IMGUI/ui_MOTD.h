#ifndef IMGUI_MOTD_H
#define IMGUI_MOTD_H

#include "imgui_window.h"
#include "hud.h"
#include "cl_util.h"

class CImGuiMOTD : public IImGuiWindow
{
public:
    void Initialize();
    void VidInitialize();
    void Terminate();
    void Reset();
    void Think();
    void Draw();

    bool Active();
    bool CursorRequired();
    bool HandleKey(bool keyDown, int keyNumber, const char *bindName);

    int MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf);

    bool m_bShow;

private:
    char m_szMOTD[MAX_MOTD_LENGTH];
    int m_iLines;
    int m_iMaxLength;
    int m_iFlags;
    bool m_ShowMOTD;
    bool ignoreThisMotd;

    cvar_t *cl_hide_motd;
};

extern CImGuiMOTD m_iMOTD;
#endif // IMGUI_MOTD_H
