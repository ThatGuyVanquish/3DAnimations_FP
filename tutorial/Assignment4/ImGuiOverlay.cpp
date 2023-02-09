#pragma once
#include <filesystem>
#include "imgui.h"
static char* getResource(const char* fileName)
{
    std::filesystem::path cwd = std::filesystem::current_path() / "../../../tutorial/Assignment4/resources";
    //std::filesystem::path cwd = std::filesystem::current_path() / "../tutorial/Assignment4/resources"; works for lior
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