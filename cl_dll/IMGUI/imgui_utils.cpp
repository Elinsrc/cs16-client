#include "imgui_utils.h"
#include <string>
#include <cmath>

CImguiUtils m_ImguiUtils;

ImVec4 CImguiUtils::ColorFromCode(char code)
{
    switch(code)
    {
        case '0': return ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
        case '1': return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        case '2': return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        case '3': return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        case '4': return ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
        case '5': return ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
        case '6': return ImVec4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta
        case '7': return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
        case '8': return ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black (same as 0)
        case '9': return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red (same as 1)
        default: return ImVec4(1.0f, 0.65f, 0.0f, 1.0f); // default orange
    }
}

void CImguiUtils::TextWithColorCodes(const char* text)
{
    bool hasColorCodes = false;
    for (const char* c = text; *c; c++)
    {
        if (*c == '^' && *(c + 1))
        {
            hasColorCodes = true;
            break;
        }
    }

    if (!hasColorCodes)
    {
        ImGui::TextUnformatted(text);
        return;
    }

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 cursor = pos;

    float lineHeight = ImGui::GetTextLineHeight();
    ImVec4 defaultColor = ImVec4(1.0f, 0.65f, 0.0f, 1.0f);

    ImVec4 color = defaultColor;
    const char* ptr = text;

    while (*ptr)
    {
        if (*ptr == '\n')
        {
            cursor.x = pos.x;
            cursor.y += lineHeight;
            ptr++;
            color = defaultColor;
            continue;
        }

        if (*ptr == '^' && *(ptr + 1))
        {
            color = ColorFromCode(*(ptr + 1));
            ptr += 2;
            continue;
        }

        const char* start = ptr;
        while (*ptr && *ptr != '\n' && !(*ptr == '^' && *(ptr + 1)))
            ptr++;

        std::string seg(start, ptr - start);

        if (!seg.empty())
        {
            draw->AddText(cursor, ImGui::ColorConvertFloat4ToU32(color), seg.c_str());
            cursor.x += ImGui::CalcTextSize(seg.c_str()).x;
        }
    }

    ImGui::Dummy(ImVec2(0, cursor.y - pos.y + lineHeight));
}

float CImguiUtils::CalcTextWidthWithColorCodes(const char* text)
{
    float width = 0.0f;
    const char* ptr = text;

    while (*ptr)
    {
        if (*ptr == '^' && *(ptr + 1))
        {
            ptr += 2;
            continue;
        }

        const char* start = ptr;
        while (*ptr && !(*ptr == '^' && *(ptr + 1)))
            ++ptr;

        if (ptr > start)
        {
            ImVec2 sz = ImGui::CalcTextSize(start, ptr);
            width += sz.x;
        }
    }

    return width;
}

float CImguiUtils::DrawTextWithColorCodesAt(const ImVec2& pos, const char* text, ImVec4 defaultColor, float alphaMul)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 cursor = pos;
    ImVec4 color  = defaultColor;

    color.w *= alphaMul;
    const char* ptr = text;

    while (*ptr)
    {
        if (*ptr == '^' && *(ptr + 1))
        {
            color = ColorFromCode(*(ptr + 1));
            color.w *= alphaMul;
            ptr += 2;
            continue;
        }

        const char* start = ptr;
        while (*ptr && !(*ptr == '^' && *(ptr + 1)))
            ++ptr;

        if (ptr > start)
        {
            ImU32 colU32 = ImGui::ColorConvertFloat4ToU32(color);

            dl->AddText(cursor, colU32, start, ptr);

            ImVec2 sz = ImGui::CalcTextSize(start, ptr);
            cursor.x += sz.x;
        }
    }

    return cursor.x - pos.x;
}
