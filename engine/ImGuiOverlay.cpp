#include "ImGuiOverlay.h"




char* ImGuiOverlay::formatScore()
{
    std::string currentScoreString = std::to_string(currentScore);
    return strcpy(new char[currentScoreString.length() + 1], currentScoreString.c_str());
}

void ImGuiOverlay::startTimer(bool &animate)
{
    if (animate || !countdown)
        return;
    if (countdownTimerEnd == 0)
        countdownTimerEnd = time(nullptr) + 3;

    float width = 1600.0f, height = 900.0f;
    bool* mainMenuToggle = nullptr;
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetCursorPos(ImVec2(700.0f, 275.0f));
    auto now = time(nullptr);
    if (now < countdownTimerEnd)
    {
        auto delta = countdownTimerEnd - now;
        std::string deltaStr = std::to_string(delta);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ShowXLText(deltaStr.c_str(), "snap");
        ImGui::PopStyleColor();
    }
    else
    {
        if (now - countdownTimerEnd < 1)
        {
            ImGui::SetCursorPos(ImVec2(650.0f, 275.0f));
            ShowXLText("GO!", "arial");
        }
        else
        {
            gameTimer = time(nullptr);
            animate = true;
            countdown = false;
        }
    }
    ImGui::End();
}

void ImGuiOverlay::displayLives(int lives)
{
    ImGui::CreateContext();
    bool* livesToggle = nullptr;
    ImGui::Begin("Lives", livesToggle, MENU_FLAGS);

    int heartWidth, heartHeight, heart_nrChannels;
    char* imgPath = getResource("heart.png");
    unsigned char* data = stbi_load(imgPath, &heartWidth, &heartHeight, &heart_nrChannels, 0);
    delete imgPath;

    // Generate the OpenGL texture
    unsigned int heart;
    glGenTextures(1, &heart);
    glBindTexture(GL_TEXTURE_2D, heart);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heartWidth, heartHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    imgPath = getResource("missing_heart.png");
    data = stbi_load(imgPath, &heartWidth, &heartHeight, &heart_nrChannels, 0);
    delete imgPath;
    unsigned int missing_heart;
    glGenTextures(1, &missing_heart);
    glBindTexture(GL_TEXTURE_2D, missing_heart);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heartWidth, heartHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    ImGui::SetWindowPos("Lives", ImVec2(737.0f, 65.0f), ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Lives", ImVec2(heartWidth * 3 + 30.0f, heartHeight + 20.0f), ImGuiCond_Always);
    //ImGui::SetCursorPos(ImVec2(850.0f, 100.0f));
    for (int i = 0; i < 3; i++)
    {
        if (lives > i)
            ImGui::Image((void*)heart, ImVec2(heartWidth, heartHeight));
        else
            ImGui::Image((void*)missing_heart, ImVec2(heartWidth, heartHeight));
        if (i == 0) ImGui::SameLine(heartWidth + 14, 0.0f);
        if (i == 1) ImGui::SameLine(2 * (heartWidth + 10), 0.0f);
    }
    ImGui::End();
}

void ImGuiOverlay::Scoreboard(bool &animate)
{
    if (!animate)
        return;
    ImGui::CreateContext();
    bool* scoreboardToggle = nullptr;
    ImGui::Begin("Scoreboard", scoreboardToggle, MENU_FLAGS);

    int width, height, nrChannels;
    char* imgPath = getResource("scoreboard_bg.png");
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

    ImGui::Image((void*)textureID, ImVec2(width, height));
    ImGui::SetWindowPos("Scoreboard", ImVec2(50.0f, -20.0f), ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Scoreboard", ImVec2(width, height), ImGuiCond_Always);
    //ImGui::SetCursorPos(ImVec2(85.0f, 185.0f));
    //ImGui::SetWindowFontScale(1.3f);
    ImGui::SetCursorPos(ImVec2(120.0f, 130.0f));
    if (currentScoreFormatted == nullptr)
        currentScoreFormatted = formatScore();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,100,0,255));
    ShowMediumText(currentScoreFormatted, "snap");

    //ImGui::SetCursorPos(ImVec2(1500.0f, 30.0f));

    auto now = time(nullptr);
    auto delta = now - gameTimer + accumulatedTime; // add mechanism to pause timer until next level start?
    std::string deltaStr = std::to_string(delta + 1);
    ImGui::SameLine(1235.0f, 0.0f);
    ShowMediumText(deltaStr.c_str(), "snap");
    ImGui::PopStyleColor();
    ImGui::End();
    displayLives(currentLives);
}

void ImGuiOverlay::MainMenu(bool &animate)
{
    if (animate || countdownTimerEnd > 0)
        return;
    ImGui::CreateContext();
    bool* mainMenuToggle = nullptr;
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
        currentLevel = 1;
        currentLives = 3;
        currentScore = 0;
        countdown = true;
        countdownTimerEnd = 0;
    }
    ImGui::End();
}



void ImGuiOverlay::DeathScreen(bool &animate)
{
    if (animate || !died)
        return;
    if (deathTimerEnd == 0)
        deathTimerEnd = time(nullptr) + 2;

    float width = 1600.0f, height = 900.0f;
    bool* deathScreenToggle = nullptr;
    ImGui::Begin("DeathScreen", deathScreenToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetCursorPos(ImVec2(450.0f, 225.0f));
    auto now = time(nullptr);
    if (now < deathTimerEnd)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        std::string msg = "Score: " + std::to_string(currentScore);
        ShowXLText(msg.c_str(), "snap");
        if (currentLives == 0)
        {
            ImGui::SetCursorPos(ImVec2(350.0f, 75.0f));
            ShowXLText("GAME OVER!", "snap");
            // maybe add scoreboard function
        }
        else
        {
            ImGui::SetCursorPos(ImVec2(450.0f, 325.0f));
            msg = "Lives: " + std::to_string(currentLives);
            ShowXLText(msg.c_str(), "snap");
        }
        ImGui::PopStyleColor();
    }
    else
    {
        deathTimerEnd = 0;
        died = false;
    }
    ImGui::End();
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