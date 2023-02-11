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

static char* formatScore(int score)
{
    std::string currentScoreString = std::to_string(score);
    return strcpy(new char[currentScoreString.length() + 1], currentScoreString.c_str());
}

//static void splashScreen(const char* msg1, const char* msg2, ImVec4 col = { 0,0,0,255 })
//{
//    float width = 1600.0f, height = 900.0f;
//    bool* splashScreenToggle = nullptr;
//    ImGui::CreateContext();
//    ImGui::Begin("Splash Screen", splashScreenToggle, MENU_FLAGS);
//    ImGui::SetWindowSize("Splash Screen", ImVec2(width, height));
//    ImGui::SetWindowPos("Splash Screen", ImVec2(0, 0), ImGuiCond_Always);
//    ImGui::SetCursorPos(ImVec2(400.0f, 275.0f));
//    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(col.w, col.x, col.y, col.z));
//    ShowXLText(msg1, "arial");
//    ImGui::SetCursorPos(ImVec2(400.0f, 375.0f));
//    ShowXLText(msg2, "arial");
//    ImGui::PopStyleColor();
//    ImGui::End();
//}