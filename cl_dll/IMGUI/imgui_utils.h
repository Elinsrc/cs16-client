#pragma once
#include "imgui.h"
#include "rgb_color.h"
#include "hud.h"
#include "cl_util.h"
#include "gl_export.h"

class CImguiUtils
{
public:

    int scrWidth() const
    {
        GLint viewport[4];
        pglGetIntegerv(GL_VIEWPORT, viewport);
        return viewport[2];
    }

    int scrHeight() const
    {
        GLint viewport[4];
        pglGetIntegerv(GL_VIEWPORT, viewport);
        return viewport[3];
    }

    ImVec4 ColorFromCode(char code);
    void TextWithColorCodes(const char* text);
    float CalcTextWidthWithColorCodes(const char* text);
    float DrawTextWithColorCodesAt(const ImVec2& pos, const char* text, ImVec4 defaultColor, float alphaMul = 1.0f);
};

extern CImguiUtils m_ImguiUtils;
