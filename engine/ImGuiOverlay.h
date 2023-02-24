#pragma once
#include <filesystem>
#include "imgui.h"
#include "gl.h"
#include "../external/stb/stb_image.h"
#include "Leaderboard.h"
#include "common.h"

static void initFonts()
{
    ImGuiIO io = ImGui::GetIO();
    char* arial = getResource("fonts/ARIAL.TTF");
    io.Fonts->AddFontFromFileTTF(arial, 16.0f);
    io.Fonts->AddFontFromFileTTF(arial, 32.0f);
    io.Fonts->AddFontFromFileTTF(arial, 64.0f);
    io.Fonts->AddFontFromFileTTF(arial, 128.0f);
    char* snap = getResource("fonts/SNAP.TTF");
    io.Fonts->AddFontFromFileTTF(snap, 16.0f);
    io.Fonts->AddFontFromFileTTF(snap, 32.0f);
    io.Fonts->AddFontFromFileTTF(snap, 64.0f);
    io.Fonts->AddFontFromFileTTF(snap, 128.0f);
}

static void ShowSmallText(const char* text, std::string font)
{
    if (font == "arial")
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    else
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowMediumText(const char* text, std::string font)
{
    if (font == "arial")
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    else
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowLargeText(const char* text, std::string font)
{
    if (font == "arial")
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
    else
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[6]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowXLText(const char* text, std::string font)
{
    if (font == "arial")
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
    else
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[7]);
    ImGui::Text(text);
    ImGui::PopFont();
}

class ImGuiOverlay {

public:

    void startTimer(bool &animate);
    static void displayLives(int lives);
    void Scoreboard(bool &animate);
    char* formatScore();
    void showLeaderboard(bool& animate);
    void MainMenu(bool &animate);
    bool PauseMenu(bool &animate);
    void DeathScreen(bool &animate);
    void LevelUpScreen(bool& animate);
    void CheatScreen(bool& animate);
    void devLegends();
    void handleCheats(const std::string& cheat);
    
    static const int MENU_FLAGS =
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

    bool countdown;
    time_t countdownTimerEnd, gameTimer, accumulatedTime;
    char* currentScoreFormatted;
    int currentLives = 3, currentLevel = 1, currentScore = 0;
    time_t deathTimerEnd = 0;
    time_t levelUpEnd = 0;
    bool died = false;
    bool leveledUp = false;
    bool displayGameOver = false;
    bool grabCallbacks = false;
    bool displayMainMenu = true, displayLeaderboard = false;
    bool displayCheatWindow = false;
    bool devMode = false;
    bool paused = false;
    Leaderboard leaderboard;
    std::vector<std::string> cheatCodes = {"DEVTOOLS"};
    char name[8];
    time_t timeFromLastWASDQE = time(nullptr);

private:


};