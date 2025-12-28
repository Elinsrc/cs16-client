#include "imgui.h"
#include "imgui_utils.h"
#include "ui_MOTD.h"

#include "parsemsg.h"
#include "keydefs.h"
#include "vgui_parser.h"

#include <string.h>

#if defined(_WIN32) || defined(__vita__)
#define strcasestr strstr
#endif

CImGuiMOTD m_iMOTD;

void CImGuiMOTD::Initialize()
{
    HOOK_MESSAGE(m_iMOTD, MOTD);

    cl_hide_motd = CVAR_CREATE("cl_hide_motd", "0", FCVAR_ARCHIVE);

    Reset();
}

void CImGuiMOTD::VidInitialize()
{
}

void CImGuiMOTD::Terminate()
{
}

void CImGuiMOTD::Reset()
{
    m_szMOTD[0] = 0;
    m_iLines = 0;
    m_iMaxLength = 0;
    m_iFlags = 0;
    m_bShow = false;
    m_ShowMOTD = false;
    ignoreThisMotd = false;
}

void CImGuiMOTD::Think()
{
}

void CImGuiMOTD::Draw()
{
    if (!m_bShow || !m_ShowMOTD)
        return;

    if (cl_hide_motd->value)
    {
        Reset();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 150));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 140, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 140, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 140, 0, 100));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 140, 0, 180));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 140, 0, 180));

    float scrWidth = m_ImguiUtils.scrWidth();
    float scrHeight = m_ImguiUtils.scrHeight();


    float windowWidth = scrWidth * 0.5f;
    float windowHeight = scrHeight * 0.6f;

    if (windowWidth < 400.0f)
        windowWidth = 400.0f;
    if (windowHeight < 250.0f)
        windowHeight = 250.0f;
    if (windowWidth > scrWidth - 20.0f)
        windowWidth = scrWidth - 20.0f;
    if (windowHeight > scrHeight - 40.0f)
        windowHeight = scrHeight - 40.0f;

    float windowPosX = (scrWidth - windowWidth) * 0.5f;
    float windowPosY = (scrHeight - windowHeight) * 0.5f;

    if (windowPosX < 10.0f)
        windowPosX = 10.0f;

    ImGui::SetNextWindowPos(ImVec2(windowPosX, windowPosY));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse;

    if (ImGui::Begin("##MOTD", nullptr, windowFlags))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();

        float padding = ImGui::GetStyle().WindowPadding.x;
        float lineStartX = win_pos.x + padding;
        float lineEndX = win_pos.x + win_size.x - padding;

        const char* title = gHUD.m_szServerName;
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        float titlePosX = (win_size.x - titleSize.x) * 0.5f;

        ImGui::SetCursorPosX(titlePosX);
        m_ImguiUtils.TextWithColorCodes(title);

        ImGui::Spacing();

        ImVec2 line_start = ImGui::GetCursorScreenPos();
        line_start.x = lineStartX;
        ImVec2 line_end = ImVec2(lineEndX, line_start.y);
        draw_list->AddLine(line_start, line_end, IM_COL32(255, 140, 0, 255), 1.0f);

        ImGui::Spacing();
        ImGui::Spacing();

        char *ch = m_szMOTD;
        char lineBuffer[1024];

        while (*ch)
        {
            char *lineEnd = ch;
            int lineLen = 0;

            while (*lineEnd != '\n' && *lineEnd != 0 && lineLen < 1023)
            {
                lineBuffer[lineLen++] = *lineEnd++;
            }
            lineBuffer[lineLen] = 0;

            m_ImguiUtils.TextWithColorCodes(lineBuffer);

            ch = lineEnd;
            if (*ch == '\n')
                ch++;
        }

        float buttonWidth = windowWidth * 0.3f;
        float minButtonWidth = 100.0f;
        float maxButtonWidth = 200.0f;

        if (buttonWidth < minButtonWidth)
            buttonWidth = minButtonWidth;
        if (buttonWidth > maxButtonWidth)
            buttonWidth = maxButtonWidth;

        float buttonHeight = scrHeight * 0.04f;
        float minButtonHeight = 25.0f;
        float maxButtonHeight = 45.0f;

        if (buttonHeight < minButtonHeight)
            buttonHeight = minButtonHeight;
        if (buttonHeight > maxButtonHeight)
            buttonHeight = maxButtonHeight;

        float bottomPadding = scrHeight * 0.01f;
        if (bottomPadding < 5.0f)
            bottomPadding = 5.0f;
        if (bottomPadding > 15.0f)
            bottomPadding = 15.0f;

        float buttonPosY = win_size.y - buttonHeight - ImGui::GetStyle().WindowPadding.y - bottomPadding;
        float buttonPosX = (win_size.x - buttonWidth) * 0.5f;

        ImGui::SetCursorPos(ImVec2(buttonPosX, buttonPosY));

        float lineOffsetY = scrHeight * 0.015f;
        if (lineOffsetY < 8.0f)
            lineOffsetY = 8.0f;
        if (lineOffsetY > 15.0f)
            lineOffsetY = 15.0f;

        ImVec2 btn_line_start = ImVec2(lineStartX, win_pos.y + buttonPosY - lineOffsetY);
        ImVec2 btn_line_end = ImVec2(lineEndX, btn_line_start.y);
        draw_list->AddLine(btn_line_start, btn_line_end, IM_COL32(255, 140, 0, 255), 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        if (ImGui::Button(Localize("Cstrike_TitlesTXT_Menu_OK"), ImVec2(buttonWidth, buttonHeight)))
        {
            m_ShowMOTD = false;
        }
        ImGui::PopStyleColor();
    }
    ImGui::End();

    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(5);
}

bool CImGuiMOTD::Active()
{
    return m_ShowMOTD;
}

bool CImGuiMOTD::CursorRequired()
{
    return true;
}

bool CImGuiMOTD::HandleKey(bool keyDown, int keyNumber, const char *bindName)
{
    if (keyNumber == K_ESCAPE || keyNumber == K_ENTER)
    {
        m_ShowMOTD = false;
    }
    return false;
}

int CImGuiMOTD::MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf)
{
    if (cl_hide_motd->value)
        return 1;

    if (m_bShow)
    {
        Reset();
    }

    if (ignoreThisMotd)
        return 1;

    BufferReader reader(pszName, pbuf, iSize);

    int is_finished = reader.ReadByte();
    strcat(m_szMOTD, reader.ReadString());

    if (strcasestr(m_szMOTD, "<!DOCTYPE HTML>"))
    {
        Reset();
        ignoreThisMotd = true;
        return 1;
    }

    if (is_finished)
    {
        int length = 0;

        m_iMaxLength = 0;
        m_iFlags |= HUD_DRAW;

        for (char *sz = m_szMOTD; *sz != 0; sz++)
        {
            if (*sz == '\n')
            {
                m_iLines++;
                if (length > m_iMaxLength)
                {
                    m_iMaxLength = length;
                    length = 0;
                }
            }
            length++;
        }

        m_iLines++;
        if (length > m_iMaxLength)
        {
            m_iMaxLength = length;
        }
        m_bShow = true;
        m_ShowMOTD = true;
    }

    return 1;
}
