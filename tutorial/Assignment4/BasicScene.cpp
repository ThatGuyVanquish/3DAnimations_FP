#include "BasicScene.h"
#include "stb_image.h"
//#include <filesystem>
#include <thread>
#include <chrono>
using namespace cg3d;

BasicScene::BasicScene(std::string name, cg3d::Display* display) : SceneWithImGui(std::move(name), display)
{
    initFonts();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void BasicScene::formatScore()
{
    if (currentScoreFormatted != nullptr)
        delete currentScoreFormatted;
    std::string currentScoreString = std::to_string(currentScore);
    currentScoreFormatted = strcpy(new char[currentScoreString.length() + 1], currentScoreString.c_str());
}

void BasicScene::startTimer()
{
    if (started)
        return;
    if (!timerRunning)
    {
        startTimerDeadline = std::chrono::high_resolution_clock::now() + std::chrono::seconds(3);
        timerRunning = true;
    }
    /*
        Initiate 3 second timer then set animate to true and start scoreboard timer
    */
    float width = 1000.0f, height = 500.0f;
    ImGui::Begin("StartTimer", mainMenuTogle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(675.0f, 275.0f));
    auto now = std::chrono::high_resolution_clock::now();
    if (now < startTimerDeadline)
    {
        auto delta = std::chrono::duration_cast<std::chrono::seconds>(startTimerDeadline - now);
        std::string deltaStr = std::to_string(delta.count() + 1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ShowXLText(deltaStr.c_str());
        ImGui::PopStyleColor();
    }
    else
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - startTimerDeadline).count() <= 500)
            ShowXLText("GO!");
        else
        {
            started = true;
            timerRunning = false;
            startOfTimer = std::chrono::high_resolution_clock::now();
        }
    }
    ImGui::End();
}

void BasicScene::Scoreboard()
{
    if (!gaming)
        return;
    float width = 1920.0, height = 100.0;
    ImGui::CreateContext();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.12f));
    ImGui::Begin("Scoreboard", scoreboardToggle,(MENU_FLAGS - ImGuiWindowFlags_NoBackground));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor();
    ImGui::SameLine(200.0f, 3.0f);
    ImGui::SetCursorPos(ImVec2(120.0f, 30.0f));
    ShowLargeText("SCORE:");
    ImGui::SameLine(0.0f, 50.0f);
    if (currentScoreFormatted == nullptr)
        formatScore();
    ShowLargeText(currentScoreFormatted);

    ImGui::SetCursorPos(ImVec2(1500.0f, 30.0f));
    ShowLargeText("TIMER:");
    auto now = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - startOfTimer);
    std::string deltaStr = std::to_string(delta.count() + 1);
    ImGui::SameLine(0.0f, 50.0f);
    ShowLargeText(deltaStr.c_str());
    ImGui::End();
}

void BasicScene::MainMenu()
{
    ImGui::CreateContext();
    ImGui::Begin("Main Menu", mainMenuTogle, MENU_FLAGS);
    
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

void BasicScene::BuildImGui()
{
    if (showMainMenu)
        MainMenu();
    if (gaming)
        Scoreboard();
    if (!started)
        startTimer();
}

void BasicScene::InitCameras(float fov, int width, int height, float near, float far)
{
    for (int i = 0; i < 2; i++)
    {
        cameras[i] = Camera::Create("camera " + std::to_string(i), fov, float(width) / float(height), near, far);
    }
    //set up camera location
    camera = cameras[0];
    //maybe place snake then have camera[1] relative to it based on being a child of snake's root
    InitSnake();
    cyls[0]->AddChild(cameras[1]);
    cameras[1]->Translate(0.5f, Axis::Y);
}

void BasicScene::InitSnake(int num)
{   
    auto cylMesh = IglLoader::MeshFromFiles("cyl_igl","data/xCylinder.obj");
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("material", program); // empty material
    for(int i = 0; i < num; i++) 
    {
        cyls.push_back(Model::Create("Cyl " + std::to_string(i), cylMesh, material));
        if (i == 0) // first axis and cylinder depend on scene's root
        {
            root->AddChild(cyls[0]);
            cyls[0]->Translate({ 0.8f, 0, 0 });
            cyls[0]->SetCenter(Eigen::Vector3f(-0.8f, 0, 0));
        }
        else
        {
            cyls[i - 1]->AddChild(cyls[i]);
            //cyls[i]->Scale(1, Axis::X);
            cyls[i]->Translate(1.6f, Axis::X);
            cyls[i]->SetCenter(Eigen::Vector3f(-0.8f, 0, 0));
        }
    }
}

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    AddChild(root = Movable::Create("root")); // a common invisible parent object for all the shapes
    InitCameras(fov, width, height, near, far);
   /* auto underwater{ std::make_shared<Material>("underwater", "shaders/cubemapShader") };
    underwater->AddTexture(0, "textures/cubemaps/Underwater_", 3);
    auto bg{ Model::Create("background", Mesh::Cube(), underwater) };
    AddChild(bg);
    bg->Scale(120, Axis::XYZ);
    bg->SetPickable(false);
    bg->SetStatic();*/

    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("material", program); // empty material

    //auto snakeShader = std::make_shared<Program>("shaders/overlay");
    //auto snakeSkin = std::make_shared<Material>("snakeSkin", snakeShader);
    //snakeSkin->AddTexture(0, "textures/snake1.png", 2);
    //auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake2.obj");
    //auto snake = Model::Create("SSSSSSSSSSSSSSSSSSSNAKE", snakeMesh, snakeSkin);
    //AddChild(snake);
    //snake->Scale(0.5);
    camera->Translate(15, Axis::Z);
    //cube->Scale(3);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
}

Eigen::Vector3f dir = { 1,0,0 };

void BasicScene::KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_R:
            showMainMenu = true;
            break;
        case GLFW_KEY_1:
            camera = cameras[1];
            break;
        case GLFW_KEY_0:
            camera = cameras[0];
            break;
        case GLFW_KEY_S:
            animate = !animate;
            break;
        case GLFW_KEY_J:
            currentScore += 1000;
            formatScore();
            break;
        case GLFW_KEY_T:
            //startTimer();
            started = false;
            break;
        }
        
    }
}
