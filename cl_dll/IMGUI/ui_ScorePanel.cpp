#include "imgui.h"
#include "imgui_utils.h"
#include "ui_ScorePanel.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "triangleapi.h"
#include "com_weapons.h"
#include "cdll_dll.h"

#include <string.h>
#include <stdio.h>
#include "draw_util.h"
#include "vgui_parser.h"
#include <ctype.h>

hud_player_info_t   g_PlayerInfoList[MAX_PLAYERS+1];
extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS+1];
team_info_t         g_TeamInfo[MAX_TEAMS+1];
hostage_info_t      g_HostageInfo[MAX_HOSTAGES+1];
int g_iUser1;
int g_iUser2;
int g_iUser3;
int g_iTeamNumber;

CImGuiScoreboard m_iScoreboard;

void CImGuiScoreboard::Initialize()
{
    HOOK_COMMAND( m_iScoreboard, "+showscores", ShowScores );
    HOOK_COMMAND( m_iScoreboard, "-showscores", HideScores );

    HOOK_MESSAGE( m_iScoreboard, ScoreInfo );
    HOOK_MESSAGE( m_iScoreboard, TeamScore );
    HOOK_MESSAGE( m_iScoreboard, TeamInfo );

    InitHUDData();
}

void CImGuiScoreboard::InitHUDData()
{
    memset( g_PlayerExtraInfo, 0, sizeof g_PlayerExtraInfo );
    m_iLastKilledBy = 0;
    m_fLastKillTime = 0;
    m_iPlayerNum = 0;
    m_iNumTeams = 0;
    memset( g_TeamInfo, 0, sizeof g_TeamInfo );

    for ( int i = 1; i <= MAX_PLAYERS; i++ )
    {
        g_PlayerExtraInfo[i].sb_health = -1;
        g_PlayerExtraInfo[i].sb_account = -1;
    }
}

void CImGuiScoreboard::VidInitialize()
{
}

void CImGuiScoreboard::Terminate()
{
}

void CImGuiScoreboard::Think()
{
}

bool CImGuiScoreboard::ShouldDrawScoreboard() const
{
    if( m_bShowscoresHeld || gHUD.m_Health.m_iHealth <= 0 || gHUD.m_iIntermission )
        return true;

    return false;
}

void CImGuiScoreboard::GetAllPlayersInfo()
{
    for ( int i = 1; i < MAX_PLAYERS; i++ )
    {
        GetPlayerInfo( i, &g_PlayerInfoList[i] );

        if ( g_PlayerInfoList[i].thisplayer )
            m_iPlayerNum = i;
    }
}

void CImGuiScoreboard::DrawPlayers( const char *team )
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    while ( 1 )
    {
        int highest_frags = -99999;
        int lowest_deaths = 99999;
        int best_player = 0;

        for ( int i = 1; i < MAX_PLAYERS; i++ )
        {
            if ( g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags )
            {
                if ( !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )
                {
                    extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
                    if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
                    {
                        best_player = i;
                        lowest_deaths = pl_info->deaths;
                        highest_frags = pl_info->frags;
                    }
                }
            }
        }

        if ( !best_player )
            break;

        hud_player_info_t *pl_info = &g_PlayerInfoList[best_player];

        int r = 255, g = 255, b = 255;
        float *colors = GetClientColor( best_player );
        r *= colors[0];
        g *= colors[1];
        b *= colors[2];

        ImVec4 playerColor = ImVec4(r/255.f, g/255.f, b/255.f, 1.0f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImVec2 row_min = ImGui::GetCursorScreenPos();
        row_min.x = m_fLineStartX;
        ImVec2 row_max = ImVec2(m_fLineEndX, row_min.y + ImGui::GetTextLineHeightWithSpacing());

        if( pl_info->thisplayer )
        {
            draw_list->AddRectFilled(row_min, row_max, IM_COL32(255, 255, 255, 15), 0.f);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
        m_ImguiUtils.TextWithColorCodes(pl_info->name);
        ImGui::PopStyleColor();

        if( team && stricmp( team, "SPECTATOR" ))
        {
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
            if( g_PlayerExtraInfo[best_player].dead )
                ImGui::Text("%s", Localize( "#Cstrike_DEAD" ));
            else if( g_PlayerExtraInfo[best_player].has_c4 )
                ImGui::Text("%s", Localize( "#Cstrike_BOMB" ));
            else if( g_PlayerExtraInfo[best_player].vip )
                ImGui::Text("%s", Localize( "#Cstrike_VIP" ));
            else if( g_PlayerExtraInfo[best_player].has_defuse_kit )
                ImGui::Text("%s", Localize( "#Cstrike_DEFUSE_KIT" ));
            ImGui::PopStyleColor();
        }

        if ( g_PlayerExtraInfo[best_player].sb_health >= 0 && !g_PlayerExtraInfo[best_player].dead )
        {
            if ( gHUD.m_pShowHealth->value )
            {
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
                ImGui::Text("%d", g_PlayerExtraInfo[best_player].sb_health);
                ImGui::PopStyleColor();
            }
        }

        if ( g_PlayerExtraInfo[best_player].sb_account >= 0 )
        {
            if ( gHUD.m_pShowMoney->value )
            {
                ImGui::TableSetColumnIndex(3);
                ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
                ImGui::Text("$%d", g_PlayerExtraInfo[best_player].sb_account);
                ImGui::PopStyleColor();
            }
        }

        if( team && stricmp( team, "SPECTATOR" ) )
        {
            ImGui::TableSetColumnIndex(4);
            ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
            ImGui::Text("%d", g_PlayerExtraInfo[best_player].frags);
            ImGui::PopStyleColor();

            ImGui::TableSetColumnIndex(5);
            ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
            ImGui::Text("%d", g_PlayerExtraInfo[best_player].deaths);
            ImGui::PopStyleColor();
        }

        ImGui::TableSetColumnIndex(6);
        ImGui::PushStyleColor(ImGuiCol_Text, playerColor);
        const char *value;
        if( pl_info->ping <= 5
            && ( value = gEngfuncs.PlayerInfo_ValueForKey( best_player, "*bot" ) )
            && atoi( value ) > 0 )
        {
            ImGui::Text("BOT");
        }
        else
        {
            ImGui::Text("%d", pl_info->ping);
        }
        ImGui::PopStyleColor();

        pl_info->name = NULL;
    }
}

void CImGuiScoreboard::DrawTeams()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    int j;

    for ( int i = 1; i <= m_iNumTeams; i++ )
    {
        if ( !g_TeamInfo[i].scores_overriden )
            g_TeamInfo[i].frags = g_TeamInfo[i].deaths = 0;
        g_TeamInfo[i].sumping = 0;
        g_TeamInfo[i].players = 0;
        g_TeamInfo[i].already_drawn = FALSE;
    }

    for ( int i = 1; i < MAX_PLAYERS; i++ )
    {
        if ( !g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0] )
            continue;

        if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
            continue;

        for ( j = 1; j <= m_iNumTeams; j++ )
        {
            if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
                break;
        }

        if ( j > m_iNumTeams )
            continue;

        if ( !g_TeamInfo[j].scores_overriden )
        {
            g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
            g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
        }

        g_TeamInfo[j].sumping += g_PlayerInfoList[i].ping;

        if ( g_PlayerInfoList[i].thisplayer )
            g_TeamInfo[j].ownteam = TRUE;
        else
            g_TeamInfo[j].ownteam = FALSE;

        g_TeamInfo[j].players++;
    }

    int iSpectatorPos = -1;

    while( true )
    {
        int highest_frags = -99999;
        int lowest_deaths = 99999;
        int best_team = 0;

        for ( int i = 1; i <= m_iNumTeams; i++ )
        {
            if ( g_TeamInfo[i].players <= 0 )
                continue;

            if ( !strnicmp(g_TeamInfo[i].name, "SPECTATOR", MAX_TEAM_NAME) )
            {
                iSpectatorPos = i;
                continue;
            }

            if ( !g_TeamInfo[i].already_drawn && g_TeamInfo[i].frags >= highest_frags )
            {
                if ( g_TeamInfo[i].frags > highest_frags || g_TeamInfo[i].deaths < lowest_deaths )
                {
                    best_team = i;
                    lowest_deaths = g_TeamInfo[i].deaths;
                    highest_frags = g_TeamInfo[i].frags;
                }
            }
        }

        if ( !best_team )
        {
            if( iSpectatorPos != -1 && g_TeamInfo[iSpectatorPos].already_drawn == FALSE )
                best_team = iSpectatorPos;
            else
                break;
        }

        team_info_t *team_info = &g_TeamInfo[best_team];

        if ( team_info->players <= 0 )
            continue;

        int r, g, b;
        char teamName[64];

        GetTeamColor( r, g, b, team_info->teamnumber );

        switch ( team_info->teamnumber )
        {
            case TEAM_TERRORIST:
                snprintf( teamName, sizeof( teamName ), "%s - %d %s", Localize( "#Cstrike_ScoreBoard_Ter" ), team_info->players, team_info->players == 1 ? "player" : "players" );
                break;
            case TEAM_CT:
                snprintf( teamName, sizeof( teamName ), "%s - %d %s", Localize( "#Cstrike_ScoreBoard_CT" ), team_info->players, team_info->players == 1 ? "player" : "players" );
                break;
            case TEAM_SPECTATOR:
            case TEAM_UNASSIGNED:
                strncpy( teamName, Localize( "#Spectators" ), sizeof( teamName ) - 1 );
                teamName[sizeof( teamName ) - 1] = '\0';
                break;
        }

        ImVec4 teamColor = ImVec4(r/255.f, g/255.f, b/255.f, 1.0f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::PushStyleColor(ImGuiCol_Text, teamColor);
        ImGui::Text("%s", teamName);
        ImGui::PopStyleColor();

        if ( team_info->teamnumber == TEAM_TERRORIST || team_info->teamnumber == TEAM_CT )
        {
            ImGui::TableSetColumnIndex(4);
            ImGui::PushStyleColor(ImGuiCol_Text, teamColor);
            ImGui::Text("%d", team_info->frags);
            ImGui::PopStyleColor();
        }

        ImGui::TableSetColumnIndex(6);
        ImGui::PushStyleColor(ImGuiCol_Text, teamColor);
        ImGui::Text("%d", team_info->sumping / team_info->players);
        ImGui::PopStyleColor();

        team_info->already_drawn = TRUE;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImVec2 line_start = ImGui::GetCursorScreenPos();
        line_start.x = m_fLineStartX;
        line_start.y -= ImGui::GetStyle().CellPadding.y;
        ImVec2 line_end = ImVec2(m_fLineEndX, line_start.y);
        draw_list->AddLine(line_start, line_end, IM_COL32(r, g, b, 255), 1.0f);

        DrawPlayers( team_info->name );

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
    }

    DrawPlayers( "" );
}

void CImGuiScoreboard::DrawScoreboard()
{
    GetAllPlayersInfo();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 2));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.55f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 140, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

    float scrWidth = m_ImguiUtils.scrWidth();
    float scrHeight = m_ImguiUtils.scrHeight();

    const char* hpText = Localize("#Cstrike_HEALTH");
    const char* moneyText = Localize("#Cstrike_ACCOUNT");
    const char* scoreText = Localize("#PlayerScore");
    const char* deathText = Localize("#PlayerDeath");
    const char* pingText = Localize("#PlayerPing");
    const char* defuseText = Localize("#Cstrike_DEFUSE_KIT");

    float colAttrib = ImGui::CalcTextSize(defuseText).x + 16.f;
    float colHP = ImGui::CalcTextSize(hpText).x + 16.f;
    float colMoney = ImGui::CalcTextSize("$16000").x + 16.f;
    float colScore = ImGui::CalcTextSize(scoreText).x + 16.f;
    float colDeath = ImGui::CalcTextSize(deathText).x + 16.f;
    float colPing = ImGui::CalcTextSize(pingText).x + 16.f;

    float minNameWidth = 150.f;
    float fixedColumnsWidth = colAttrib + colHP + colMoney + colScore + colDeath + colPing;
    float minWindowWidth = fixedColumnsWidth + minNameWidth + 40.f;

    float padding_y = scrHeight * 0.05f;
    float sb_width = scrWidth * 0.55f;

    if (sb_width < minWindowWidth)
        sb_width = minWindowWidth;

    if (sb_width > scrWidth - 20.f)
        sb_width = scrWidth - 20.f;

    float sb_height = scrHeight - 2 * padding_y;
    float sb_x = (scrWidth - sb_width) / 2;
    float sb_y = padding_y;

    if (sb_x < 10.f)
        sb_x = 10.f;

    ImGui::SetNextWindowPos(ImVec2(sb_x, sb_y));
    ImGui::SetNextWindowSize(ImVec2(sb_width, sb_height));

    if (ImGui::Begin("##Scoreboard", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoInputs))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();

        float padding = ImGui::GetStyle().WindowPadding.x;
        m_fLineStartX = win_pos.x + padding;
        m_fLineEndX = win_pos.x + win_size.x - padding;

        if (ImGui::BeginTable("ScoreboardTable", 7, ImGuiTableFlags_SizingFixedFit, ImVec2(0, sb_height - 20)))
        {
            ImGui::TableSetupColumn("##ServerName", ImGuiTableColumnFlags_WidthStretch, 0.f);
            ImGui::TableSetupColumn("##Attrib", ImGuiTableColumnFlags_WidthFixed, colAttrib);
            ImGui::TableSetupColumn("##HP", ImGuiTableColumnFlags_WidthFixed, colHP);
            ImGui::TableSetupColumn("##Money", ImGuiTableColumnFlags_WidthFixed, colMoney);
            ImGui::TableSetupColumn("##Score", ImGuiTableColumnFlags_WidthFixed, colScore);
            ImGui::TableSetupColumn("##Death", ImGuiTableColumnFlags_WidthFixed, colDeath);
            ImGui::TableSetupColumn("##Ping", ImGuiTableColumnFlags_WidthFixed, colPing);

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableSetColumnIndex(0);
            ImVec2 headerPos = ImGui::GetCursorScreenPos();
            m_ImguiUtils.DrawTextWithColorCodesAt(headerPos, gHUD.m_szServerName, ImVec4(1.0f, 0.55f, 0.0f, 1.0f), 1.0f);

            ImGui::TableSetColumnIndex(1);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", hpText);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", moneyText);

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%s", scoreText);

            ImGui::TableSetColumnIndex(5);
            ImGui::Text("%s", deathText);

            ImGui::TableSetColumnIndex(6);
            ImGui::Text("%s", pingText);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImVec2 line_start = ImGui::GetCursorScreenPos();
            line_start.x = m_fLineStartX;
            ImVec2 line_end = ImVec2(m_fLineEndX, line_start.y);
            draw_list->AddLine(line_start, line_end, IM_COL32(255, 140, 0, 255), 1.0f);
            ImGui::Spacing();

            if ( gHUD.m_Teamplay )
            {
                DrawTeams();
            }
            else
            {
                DrawPlayers( NULL );
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(6);
}

void CImGuiScoreboard::Draw()
{
    if( !ShouldDrawScoreboard() )
        return;

    DrawScoreboard();
}

bool CImGuiScoreboard::Active()
{
    return ShouldDrawScoreboard();
}

bool CImGuiScoreboard::CursorRequired()
{
    return false;
}

bool CImGuiScoreboard::HandleKey(bool keyDown, int keyNumber, const char* bindName)
{
    return true;
}

int CImGuiScoreboard::MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf )
{
    BufferReader reader( pszName, pbuf, iSize );
    short cl = reader.ReadByte();
    short frags = reader.ReadShort();
    short deaths = reader.ReadShort();
    short playerclass = reader.ReadShort();
    short teamnumber = reader.ReadShort();

    if ( cl > 0 && cl <= MAX_PLAYERS )
    {
        g_PlayerExtraInfo[cl].frags = frags;
        g_PlayerExtraInfo[cl].deaths = deaths;
        g_PlayerExtraInfo[cl].playerclass = playerclass;
        g_PlayerExtraInfo[cl].teamnumber = teamnumber;
    }

    return 1;
}

int CImGuiScoreboard::MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf )
{
    BufferReader reader( pszName, pbuf, iSize );
    short cl = reader.ReadByte();
    int teamNumber = 0;

    if ( cl > 0 && cl <= MAX_PLAYERS )
    {
        char teamName[MAX_TEAM_NAME];
        strncpy( teamName, reader.ReadString(), MAX_TEAM_NAME );
        teamName[MAX_TEAM_NAME-1] = 0;

        if( !strcmp( teamName, "TERRORIST") )
            teamNumber = TEAM_TERRORIST;
        else if( !strcmp( teamName, "CT") )
            teamNumber = TEAM_CT;
        else if( !strcmp( teamName, "SPECTATOR" ) || !strcmp( teamName, "UNASSIGNED" ) )
        {
            teamNumber = TEAM_SPECTATOR;
            strncpy( teamName, "SPECTATOR", MAX_TEAM_NAME );
        }
        else teamNumber = TEAM_UNASSIGNED;

        strncpy( g_PlayerExtraInfo[cl].teamname, teamName, MAX_TEAM_NAME );
        g_PlayerExtraInfo[cl].teamnumber = teamNumber;
    }

    for ( int i = 1; i <= m_iNumTeams; i++ )
    {
        g_TeamInfo[i].players = 0;
    }

    GetAllPlayersInfo();
    m_iNumTeams = 0;

    for ( int i = 1; i < MAX_PLAYERS; i++ )
    {
        int j;

        if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
            continue;

        for ( j = 1; j <= m_iNumTeams; j++ )
        {
            if ( g_TeamInfo[j].name[0] == '\0' )
                break;

            if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
                break;
        }

        if ( j > m_iNumTeams )
        {
            for ( j = 1; j <= m_iNumTeams; j++ )
            {
                if ( g_TeamInfo[j].name[0] == '\0' )
                    break;
            }

            m_iNumTeams = Q_max( j, m_iNumTeams );

            strncpy( g_TeamInfo[j].name, g_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
            g_TeamInfo[j].teamnumber = g_PlayerExtraInfo[i].teamnumber;
            g_TeamInfo[j].players = 0;
        }

        g_TeamInfo[j].players++;
    }

    for ( int i = 1; i <= m_iNumTeams; i++ )
    {
        if ( g_TeamInfo[i].players < 1 )
            memset( &g_TeamInfo[i], 0, sizeof(team_info_t) );
    }

    return 1;
}

int CImGuiScoreboard::MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf )
{
    BufferReader reader( pszName, pbuf, iSize );
    char *TeamName = reader.ReadString();

    for ( int i = 1; i <= m_iNumTeams; i++ )
    {
        if ( !stricmp( TeamName, g_TeamInfo[i].name ) )
        {
            g_TeamInfo[i].scores_overriden = TRUE;
            g_TeamInfo[i].frags = reader.ReadShort();
            return 1;
        }
    }

    reader.Flush();
    return 1;
}

void CImGuiScoreboard::DeathMsg( int killer, int victim )
{
    if ( victim == m_iPlayerNum || killer == 0 )
    {
        m_iLastKilledBy = killer ? killer : m_iPlayerNum;
        m_fLastKillTime = gHUD.m_flTime + 10;

        if ( killer == m_iPlayerNum )
            m_iLastKilledBy = m_iPlayerNum;
    }
}

void CImGuiScoreboard::UserCmd_ShowScores()
{
    m_bShowscoresHeld = true;
}

void CImGuiScoreboard::UserCmd_HideScores()
{
    m_bShowscoresHeld = false;
}
