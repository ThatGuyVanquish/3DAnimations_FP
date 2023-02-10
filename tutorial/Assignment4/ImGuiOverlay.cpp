#pragma once
#include <filesystem>
#include "imgui.h"
#include "gl.h"
#include "stb_image.h"

static const int MENU_FLAGS =
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;


static char* getResource(const char* fileName)
{
//    std::filesystem::path cwd = std::filesystem::current_path() / "../../../tutorial/Assignment4/resources";
    std::filesystem::path cwd = std::filesystem::current_path() / "../tutorial/Assignment4/resources"; // works for lior
    std::filesystem::path filePath = cwd / fileName;
    std::string filePathString = filePath.string();
    return strcpy(new char[filePathString.length() + 1], filePathString.c_str());
}

static void initFonts()
{
    ImGuiIO io = ImGui::GetIO();
    char* arial = getResource("ARIAL.TTF");
    io.Fonts->AddFontFromFileTTF(arial, 16.0f);
    io.Fonts->AddFontFromFileTTF(arial, 32.0f);
    io.Fonts->AddFontFromFileTTF(arial, 64.0f);
    io.Fonts->AddFontFromFileTTF(arial, 128.0f);
}

static void ShowSmallText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowMediumText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowLargeText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static void ShowXLText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
    ImGui::Text(text);
    ImGui::PopFont();
}

static char* formatScore(int score)
{
    std::string currentScoreString = std::to_string(score);
    return strcpy(new char[currentScoreString.length() + 1], currentScoreString.c_str());
}

static void startTimer(bool &started, bool &timerRunning, time_t &startTimerDeadline, time_t &startOfTimer)
{
    if (started)
        return;
    if (!timerRunning)
    {
        startTimerDeadline = time(nullptr) + 3;
        timerRunning = true;
    }
    /*
        Initiate 3 second timer then set animate to true and start scoreboard timer
    */
    float width = 1000.0f, height = 500.0f;
    bool *mainMenuToggle = nullptr;
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(675.0f, 275.0f));
    auto now = time(nullptr);
    if (now < startTimerDeadline)
    {
        auto delta = startTimerDeadline - now;
        std::string deltaStr = std::to_string(delta + 1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ShowXLText(deltaStr.c_str());
        ImGui::PopStyleColor();
    }
    else
    {
        if ((now - startTimerDeadline) < 1)
            ShowXLText("GO!");
        else
        {
            started = true;
            timerRunning = false;
            startOfTimer = time(nullptr);
        }
    }
    ImGui::End();
}

static void Scoreboard(bool &gaming, char *currentScoreFormatted, time_t &startOfTimer)
{
    if (!gaming)
        return;
    float width = 1920.0, height = 100.0;
    ImGui::CreateContext();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.12f));
    bool* scoreboardToggle = nullptr;
    ImGui::Begin("Scoreboard", scoreboardToggle,(MENU_FLAGS - ImGuiWindowFlags_NoBackground));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor();
    ImGui::SameLine(200.0f, 3.0f);
    ImGui::SetCursorPos(ImVec2(120.0f, 30.0f));
    ShowLargeText("SCORE:");
    ImGui::SameLine(0.0f, 50.0f);
    if (currentScoreFormatted == nullptr)
        currentScoreFormatted = formatScore(0);
    ShowLargeText(currentScoreFormatted);

    ImGui::SetCursorPos(ImVec2(1500.0f, 30.0f));
    ShowLargeText("TIMER:");
    auto now = time(nullptr);
    auto delta = now - startOfTimer;
    std::string deltaStr = std::to_string(delta + 1);
    ImGui::SameLine(0.0f, 50.0f);
    ShowLargeText(deltaStr.c_str());
    ImGui::End();
}

static void MainMenu(bool &showMainMenu, bool &gaming, bool &started)
{
    ImGui::CreateContext();
    bool *mainMenuToggle = nullptr;
    ImGui::Begin("Main Menu", mainMenuToggle, MENU_FLAGS);

    int width, height, nrChannels;
    char* imgPath = getResource("mainmenu_bg.png");
    unsigned char* data = stbi_load(imgPath, &width, &height, &nrChannels, 0);
    delete imgPath;

    // Generate the OpenGL texture
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    ImGui::Image((void*)textureID, ImVec2(width,height));
    ImGui::SetWindowPos("Main Menu", ImVec2(675.0f, 275.0f), ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Main Menu", ImVec2(width + 20, height + 20), ImGuiCond_Always);
    ImGui::SetCursorPos(ImVec2(85.0f, 185.0f));
    ImGui::SetWindowFontScale(1.3f);
    if (ImGui::Button("START GAME"))
    {
        showMainMenu = false;
        gaming = true;
        started = false;
    }
    ImGui::End();
}