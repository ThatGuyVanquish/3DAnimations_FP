#include "ImGuiOverlay.h"
#include <Gameplay.h>




char* ImGuiOverlay::formatScore()
{
    std::string currentScoreString = std::to_string(currentScore);
    return strcpy(new char[currentScoreString.length() + 1], currentScoreString.c_str());
}

void ImGuiOverlay::startTimer(bool &animate)
{
    if (animate || !countdown || died || leveledUp)
        return;
    if (countdownTimerEnd == 0)
        countdownTimerEnd = time(nullptr) + 3;

    float width = 1600.0f, height = 900.0f;
    bool* mainMenuToggle = nullptr;
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= 100;
    center.y -= 120;
    ImGui::SetCursorPos(center);
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
            ImGui::SetCursorPos(ImVec2(center.x - 50.0f, center.y));
            ShowXLText("GO!", "arial");
        }
        else
        {
            gameTimer = timeFromLastWASDQE = time(nullptr);
            animate = true;
            countdown = false;
            grabCallbacks = true;
        }
    }
    ImGui::End();
}

void ImGuiOverlay::displayLives(int lives)
{
    ImGui::CreateContext();
    bool* livesToggle = nullptr;
    ImGui::Begin("Lives", livesToggle, MENU_FLAGS);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.y = 65.0f;
    center.x -= 70.0f;
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

    ImGui::SetWindowPos("Lives", center, ImGuiCond_Always);
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
    if (!animate || died)
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

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= width/2;
    center.y = -20;

    ImGui::Image((void*)textureID, ImVec2(width, height));
    ImGui::SetWindowPos("Scoreboard", center, ImGuiCond_Always);
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

void ImGuiOverlay::showLeaderboard(bool &animate)
{
    if (animate || !displayLeaderboard)
        return;
    ImGui::CreateContext();
    bool* leaderboardToggle = nullptr;
    ImGui::Begin("Leaderboard", leaderboardToggle, MENU_FLAGS);

    int width, height, nrChannels;
    char* imgPath = getResource("leaderboard_bg.png");
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
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= width/2;
    center.y -= height/2;

    ImGui::Image((void*)textureID, ImVec2(width, height));
    ImGui::SetWindowPos("Leaderboard", center, ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Leaderboard", ImVec2(width + 20, height + 20), ImGuiCond_Always);
    auto entries = leaderboard.getEntries();
    float yPos = 140.0f;
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,100,0,255));
    for (int i = 0; i < entries.size(); i++)
    {
        ImGui::SetCursorPos(ImVec2(70.0f, yPos + 20 * i));
        std::string entry = std::to_string(i + 1) + ".  " + entries[i].name + "   " + std::to_string(entries[i].points);
        ShowSmallText(entry.c_str(), "snap");
    }
    ImGui::PopStyleColor();
    ImGui::SetCursorPos(ImVec2(130.0f, yPos + entries.size() * 20 + 10));
    ImGui::SetItemAllowOverlap();
    ImGui::PushStyleColor(ImGuiCol_Button, GL_CLEAR);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,100,0,255));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    if (ImGui::Button("BACK"))
    {
        displayLeaderboard = false;
        displayMainMenu = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(2);
    ImGui::End();
}

void ImGuiOverlay::MainMenu(bool &animate)
{
    if (animate || countdownTimerEnd > 0 || (died && !displayGameOver) || leveledUp)
        return;
    if (!displayMainMenu)
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
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= width/2;
    center.y -= height/2;
    //ImGui::SetWindowPos("Main Menu", ImVec2(675.0f, 275.0f), ImGuiCond_Always);
    ImGui::SetWindowPos("Main Menu", center, ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Main Menu", ImVec2(width + 20, height + 20), ImGuiCond_Always);
    ImGui::SetCursorPos(ImVec2(85.0f, 185.0f));
    ImGui::SetWindowFontScale(1.3f);
    ImGui::PushStyleColor(ImGuiCol_Button, GL_CLEAR);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,100,0,255));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    if (ImGui::Button("START GAME"))
    {
        callPythonScript("scripts/play_sound.py", "audio/here_we_go.mp3", 2);
        currentLevel = 1;
        currentLives = 3;
        currentScore = 0;
        currentScoreFormatted = formatScore();
        countdown = true;
        displayGameOver = false;
        died = false;
        countdownTimerEnd = 0;
        deathTimerEnd = 0;
    }
    ImGui::SetCursorPos(ImVec2(85.0f, 220.0f));
    if (ImGui::Button("LEADERBOARD"))
    {
        displayMainMenu = false;
        displayLeaderboard = true;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
}

void ImGuiOverlay::DeathScreen(bool &animate)
{
    if (animate || !died)
        return;
    if (deathTimerEnd == 0)
        deathTimerEnd = time(nullptr) + 2;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= 300;
    center.y -= 200;
    bool* deathScreenToggle = nullptr;
    ImGui::Begin("DeathScreen", deathScreenToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(1600, 900));
    if (displayGameOver)
    {
        ImGui::SetCursorPos(ImVec2(center.x - 150, 75.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ShowXLText("GAME OVER!", "snap");
        ImGui::PopStyleColor();
        ImGui::End();
        return;
    }
    auto now = time(nullptr);
    if (now < deathTimerEnd)
    {
        ImGui::SetCursorPos(ImVec2(center.x, center.y - 50.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        std::string msg = "Score: " + std::to_string(currentScore);
        ShowXLText(msg.c_str(), "snap");
        if (currentLives == 0)
        {
            ImGui::SetCursorPos(ImVec2(center.x - 150, center.y + 50.0f));
            ShowXLText("GAME OVER!", "snap");
            ImGui::SetCursorPos(ImVec2(center.x + 150, center.y + 200.0f));
            ImGui::SetNextItemWidth(200.0f);
            char name[20] = "INSERT NAME HERE";
            int flags = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_EnterReturnsTrue;
            grabCallbacks = false;
            if (!ImGui::InputText("", name, 20, flags))
            {
                deathTimerEnd = time(nullptr) + 5;
                animate = false;
            }
            else
            {
                leaderboard.add(name, currentScore);
                deathTimerEnd = time(nullptr);
                displayGameOver = true;
                grabCallbacks = true;
            }
        }
        else
        {
            ImGui::SetCursorPos(ImVec2(center.x, center.y + 100.0f));
            msg = "Lives: " + std::to_string(currentLives);
            ShowXLText(msg.c_str(), "snap");
        }
        ImGui::PopStyleColor();
    }
    else
    {
        deathTimerEnd = 0;
        if (currentLives == 0)
            displayGameOver = true;
        else
            died = false;
    }
    ImGui::End();
}

void ImGuiOverlay::LevelUpScreen(bool& animate)
{
    if (animate || !leveledUp)
        return;
    if (levelUpEnd == 0)
        levelUpEnd = time(nullptr) + 2;

    float width = 1600.0f, height = 300.0f;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.y -= 200;
    center.x -= 350;
    bool* levelUpToggle = nullptr;
    ImGui::Begin("Level Up", levelUpToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos("Level Up", center);
    //ImGui::SetCursorPos(ImVec2(-200, 0));
    auto now = time(nullptr);
    if (now < levelUpEnd)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        std::string msg = "Leveled Up!";
        ShowXLText(msg.c_str(), "snap");
        ImGui::PopStyleColor();
    }
    else
    {
        levelUpEnd = 0;
        leveledUp = false;
    }
    ImGui::End();
}

void ImGuiOverlay::handleCheats(const std::string& cheat)
{
    int index = std::find(cheatCodes.begin(), cheatCodes.end(), cheat) - cheatCodes.begin();
    switch (index)
    {
        case 0:
            devMode = !devMode;
            break;
    }
}

void ImGuiOverlay::CheatScreen(bool &animate)
{
    if (animate || !displayCheatWindow)
        return;

    float width = 1600.0f, height = 900.0f;
    bool* cheatWindowToggle = nullptr;
    ImGui::Begin("CheatWindow", cheatWindowToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.y -= 300;
    center.x -= 300;
    ImGui::SetCursorPos(center);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    std::string msg = "CHEATS";
    ShowXLText(msg.c_str(), "arial");
    ImGui::SetCursorPos(ImVec2(center.x + 150, center.y + 150));
    ImGui::SetNextItemWidth(200.0f);
    char cheat[20] = "INSERT CHEATS HERE";
    int flags = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_EnterReturnsTrue;
    grabCallbacks = false;
    if (ImGui::InputText("", cheat, 20, flags))
    {
        if (std::find(cheatCodes.begin(), cheatCodes.end(), cheat) != cheatCodes.end())
            handleCheats(cheat);
        displayCheatWindow = false;
        timeFromLastWASDQE = time(nullptr);
        animate = true;
        grabCallbacks = true;
    }
    ImGui::PopStyleColor();

    ImGui::End();
}

bool ImGuiOverlay::PauseMenu(bool &animate)
{
    if (!paused)
        return false;
    if (animate)
        animate = false;
    ImGui::CreateContext();
    bool* pauseMenuToggle = nullptr;
    ImGui::Begin("Pause Menu", pauseMenuToggle, MENU_FLAGS);

    int width, height, nrChannels;
    char* imgPath = getResource("pausemenu_bg.png");
    unsigned char* data = stbi_load(imgPath, &width, &height, &nrChannels, 0);
    delete imgPath;

    // Generate the OpenGL texture
    unsigned int bgImage;
    glGenTextures(1, &bgImage);
    glBindTexture(GL_TEXTURE_2D, bgImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    ImGui::Image((void*)bgImage, ImVec2(width,height));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center.x -= width/2;
    center.y -= height/2;
    ImGui::SetWindowPos("Pause Menu", center, ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Pause Menu", ImVec2(width + 20, height + 20), ImGuiCond_Always);
    ImGui::SetCursorPos(ImVec2(85.0f, 185.0f));
    ImGui::SetWindowFontScale(1.3f);
    ImGui::PushStyleColor(ImGuiCol_Button, GL_CLEAR);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,100,0,255));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    if (ImGui::Button("RESUME"))
    {
        callPythonScript("scripts/play_sound.py", "audio/here_we_go.mp3", 2);
        countdown = true;
        countdownTimerEnd = 0;
        deathTimerEnd = 0;
        paused = false;
    }

    ImGui::SetCursorPos(ImVec2(85.0f, 220.0f));
    if (ImGui::Button("MAIN MENU"))
    {
        displayMainMenu = true;
        paused = false;
        deathTimerEnd = 0;
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGui::End();
        return true;
    }

    ImGui::SetCursorPos(ImVec2(85.0f, 255.0f));
    if (ImGui::Button("LEVIOSA?"))
    {
        callPythonScript("scripts/play_sound.py", "audio/leviosa.mp3", 3);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
    return false;
}
