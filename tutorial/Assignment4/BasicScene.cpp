#pragma once
#include "BasicScene.h"
#include "stb_image.h"
#include <chrono>
#include <filesystem>
#include <random>
#include "Gameplay.cpp"

using namespace cg3d;

BasicScene::BasicScene(std::string name, cg3d::Display* display) : SceneWithImGui(std::move(name), display)
{
    initFonts();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    InitMaterials();
    AddChild(root = Movable::Create("root")); // a common invisible parent object for all the shapes
    InitSnake();
    InitCameras(fov, width, height, near, far);
    FOV = fov; WIDTH = width; HEIGHT = height; NEAR = near; FAR = far;
    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    camera->Translate(30, Axis::Z);
    camera->Translate(15, Axis::Y);
    camera->Rotate((float)-M_PI_4/2.0, Axis::X);

    // Creation of axis mesh
    Eigen::MatrixXd vertices(6, 3);
    vertices << -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1;
    Eigen::MatrixXi faces(3, 2);
    faces << 0, 1, 2, 3, 4, 5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6, 3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6, 2);
    coordsys = std::make_shared<Mesh>("coordsys", vertices, faces, vertexNormals, textureCoords);
    auto axis = Model::Create("Axis", coordsys, basicMaterial);
    axis->mode = 1;
    axis->Scale(20);
    AddChild(axis);
    generateViableEntities(viableItems, viableEnemies, viableBonuses);
    currentLevel = 1;
    InitLevel(viableItems, viableEnemies, viableBonuses, entities,
       MAP_SIZE, basicMaterial, green, red, root, currentLevel);
}

/**
 * model init methods:
 * 1. InitMaterials
 * 2. InitCameras
 * 3. InitSnake
 * 4. initObjects
 */
void BasicScene::InitMaterials()
{
    program = std::make_shared<Program>("shaders/basicShader");
    basicMaterial = std::make_shared<Material>("basicMaterial", program); // empty material
    green = std::make_shared<Material>("green", program, true);
    green->AddTexture(0, "textures/grass.bmp", 2);
    red = std::make_shared<Material>("red", program);
    red->AddTexture(0, "textures/box0.bmp", 2);
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
    cyls[0].model->AddChild(cameras[1]);
    cameras[1]->Translate(0.5f, Axis::Y);
    cameras[1]->Translate(0.5, Axis::X);
    cameras[1]->Rotate((float)M_PI_2, Axis::Y);
}

void BasicScene::InitSnake()
{
    //init cylinders
    auto cylMesh = IglLoader::MeshFromFiles("cyl_igl","data/zCylinder.obj");
    float scaleFactor = 1.0f;
    igl::AABB<Eigen::MatrixXd, 3> cyl_aabb = CollisionDetection::InitAABB(cylMesh);
    for(int i = 0; i < numOfCyls; i++)
    {
        auto cylModel = Model::Create("Cyl " + std::to_string(i), cylMesh, basicMaterial);
        cyls.push_back({cylModel, scaleFactor, cyl_aabb});
        CollisionDetection::InitCollisionModels(cyls[i], green, red);
        cyls[i].model->showFaces = false;

        if (i == 0) // first axis and cylinder depend on scene's root
        {
            root->AddChild(cyls[0].model);
            cyls[0].model->Translate({ 0, 0, 0.8f });
            cyls[0].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        }
        else
        {
            cyls[i - 1].model->AddChild(cyls[i].model);
            //cyls[i].model->Scale(1, Axis::X);
            cyls[i].model->Translate(1.6f, Axis::Z);
            cyls[i].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        }
    }

    // init head
    auto headMesh = IglLoader::MeshFromFiles("head", "data/camelhead.off");
    auto headModel = Model::Create("head", headMesh, basicMaterial);
    igl::AABB<Eigen::MatrixXd, 3> head_aabb = CollisionDetection::InitAABB(headMesh);
    head = {headModel, 1.0f, head_aabb};
    CollisionDetection::InitCollisionModels(head, green, red);
    headModel->Scale(head.scaleFactor);
    headModel->Rotate((float)-M_PI, Axis::Y);
    headModel->Translate(-1.6f, Axis::Z);
    headModel->showFaces = false;
    headModel->showWireframe = true;
    cyls[0].model->AddChild(headModel);


    // init snake
    snakeShader = std::make_shared<Program>("shaders/overlay");
    snakeSkin = std::make_shared<Material>("snakeSkin", snakeShader);
//    snakeSkin->AddTexture(0, "textures/snake1.png", 2);
    auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake2.obj");
    auto snakeModel = Model::Create("SNAKE", snakeMesh, snakeSkin);
    igl::AABB<Eigen::MatrixXd, 3> snake_aabb = CollisionDetection::InitAABB(snakeMesh);
    snake = {snakeModel, 16.0f, snake_aabb};
    snakeModel->Translate(1.6f*(numOfCyls/2)-0.8f, Axis::Z);
    snakeModel->Scale(16.0f, Axis::Z);
    snakeModel->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
//    InitCollisionModels(head); // should we fix cylinder collision or use snake mesh? (problem with scaling in only one axis)
//    cyls[0].model->AddChild(snakeModel);

}

void BasicScene::DeleteSnake()
{
    // Remove all of the cylinders from the scene
    for (long i = cyls.size() - 1; i > 0; i--) {
        cyls[i - 1].model->RemoveChild(cyls[i].model);
        cyls.pop_back();
    }
    root->RemoveChild(cyls[0].model);
    cyls.pop_back();

    // Remove the snake itself
    root->RemoveChild(head.model);
    root->RemoveChild(snake.model);
}

void BasicScene::Reset(bool mainMenu)
{
    /*
        1. Delete the snake 
        2. Delete all of the entities
        3. Initiate the snake
        4. Delete the cameras
        5. Initialize cameras
        *6. Level should be initialized after call for start level so that
        *   entities won't pass away too quickly
    */
    animate = false;
    if (mainMenu)
    {
        countdown = false;
        countdownTimerEnd = 0;
        accumulatedTime = 0;
    }
    //root->RemoveChild(snake.model); need to create a method to remove the snake
    else
    {
        countdown = true;
        countdownTimerEnd = 0;
        accumulatedTime += time(nullptr) - gameTimer;
    }
    DeleteSnake();
    clearEntities(entities, root);
    InitSnake();
    InitCameras(FOV, WIDTH, HEIGHT, NEAR, FAR);
    InitLevel(viableItems, viableEnemies, viableBonuses, 
        entities, MAP_SIZE, basicMaterial, green, red, root, currentLevel);
}

/**
 * need to add actions to be made when a collision of 2 objects detected, like:
 * 1. when the head of the snake collides with a "good" object, it should be eaten and gain score
 * 2. when the head of the snake collides with a "bad" object or itself, it should lower the score or end the game
 * 3. any other scenarios...
 */
void BasicScene::checkForCollision()
{
    // collision with the head:
    for (int i = 1; i < cyls.size(); i++)
    {
        Eigen::AlignedBox3d box_camel, box_i;
        if (CollisionDetection::intersects(
                head.scaleFactor,
                head.aabb,
                head.model->GetAggregatedTransform(),
                cyls[i].scaleFactor,
                cyls[i].aabb,
                cyls[i].model->GetAggregatedTransform(),
                box_camel, box_i
        ))
        {
            animate = false;
            CollisionDetection::SetCollisionBox(head, box_camel);
            CollisionDetection::SetCollisionBox(cyls[i], box_i);
        }
    }
    for (int i = 0; i < objects.size(); i++)
    {
        Eigen::AlignedBox3d box_camel, box_i;
        if (CollisionDetection::intersects(
                head.scaleFactor,
                head.aabb,
                head.model->GetAggregatedTransform(),
                objects[i].scaleFactor,
                objects[i].aabb,
                objects[i].model->GetAggregatedTransform(),
                box_camel, box_i
        ))
        {
            animate = false;
            CollisionDetection::SetCollisionBox(head, box_camel);
            CollisionDetection::SetCollisionBox(objects[i], box_i);
        }
    }

    // search for collision between cylinders and between cylinders and objects:
    for (int i = 0; i < cyls.size(); i++) {
        for (int j = i+2; j < cyls.size(); j++) {
            Eigen::AlignedBox3d box_i, box_j;
            if (CollisionDetection::intersects(
                    cyls[i].scaleFactor,
                    cyls[i].aabb,
                    cyls[i].model->GetAggregatedTransform(),
                    cyls[j].scaleFactor,
                    cyls[j].aabb,
                    cyls[j].model->GetAggregatedTransform(),
                    box_i, box_j
            ))
            {
                animate = false;
                CollisionDetection::SetCollisionBox(cyls[i], box_i);
                CollisionDetection::SetCollisionBox(cyls[j], box_j);
            }
        }

        for (int j = 0; j < objects.size(); j++)
        {
            Eigen::AlignedBox3d box_i, box_j;
            if (CollisionDetection::intersects(
                    cyls[i].scaleFactor,
                    cyls[i].aabb,
                    cyls[i].model->GetAggregatedTransform(),
                    objects[j].scaleFactor,
                    objects[j].aabb,
                    objects[j].model->GetAggregatedTransform(),
                    box_i, box_j
            ))
            {
                animate = false;
                CollisionDetection::SetCollisionBox(cyls[i], box_i);
                CollisionDetection::SetCollisionBox(objects[j], box_j);
            }
        }
    }
}

void BasicScene::SetCamera(int index)
{
    camera = cameras[index];
    viewport->camera = camera;
}

void BasicScene::startTimer()
{
    if (animate || !countdown)
        return;
    if (countdownTimerEnd == 0)
        countdownTimerEnd = time(nullptr) + 3;

    float width = 1000.0f, height = 500.0f;
    bool* mainMenuToggle = nullptr;
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(675.0f, 275.0f));
    auto now = time(nullptr);
    if (now < countdownTimerEnd)
    {
        countdownTimer = time(nullptr);
        auto delta = countdownTimerEnd - now;
        std::string deltaStr = std::to_string(delta);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ShowXLText(deltaStr.c_str(), "snap");
        ImGui::PopStyleColor();
    }
    else
    {
        if (now - countdownTimerEnd < 1)
            ShowXLText("GO!", "arial");
        else
        {
            gameTimer = time(nullptr);
            animate = true;
            countdown = false;
        }
    }
    ImGui::End();
}

static void displayLives(int lives, int MENU_FLAGS)
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

void BasicScene::Scoreboard()
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
        currentScoreFormatted = formatScore(0);
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
    displayLives(currentLives, MENU_FLAGS);
}

void BasicScene::MainMenu()
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
        countdown = true;
        countdownTimerEnd = 0;
    }
    ImGui::End();
}

void BasicScene::BuildImGui()
{
    MainMenu();
    Scoreboard();
    startTimer();
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
//    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
//    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
//    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
//    program.SetUniform1f("specular_exponent", 5.0);
//    program.SetUniform4f("light_position", 0.0, 15.0, 0.0, 1.0);
}

void BasicScene::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
//    auto system = camera->GetRotation().transpose();
    auto system = cyls[0].model->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
//            cyls[0].model->RotateInSystem(system, 0.1f, Axis::Y);
//            cyls[1].model->RotateInSystem(system, -0.1f, Axis::Y);
            cyls[0].model->Rotate(0.1f, Axis::Y);
            cyls[1].model->Rotate(-0.1f, Axis::Y);
            snake.model->Rotate(-0.1f, Axis::Y);
            break;
        case GLFW_KEY_RIGHT:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Y);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Y);
            cyls[0].model->Rotate(-0.1f, Axis::Y);
            cyls[1].model->Rotate(0.1f, Axis::Y);
            snake.model->Rotate(0.1f, Axis::Y);
            break;
        case GLFW_KEY_UP:
//            cyls[0].model->RotateInSystem(system, 0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, -0.1f, Axis::Z);
            cyls[0].model->Rotate(0.1f, Axis::X);
            cyls[1].model->Rotate(-0.1f, Axis::X);
            snake.model->Rotate(-0.1f, Axis::X);
            break;
        case GLFW_KEY_DOWN:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Z);
            cyls[0].model->Rotate(-0.1f, Axis::X);
            cyls[1].model->Rotate(0.1f, Axis::X);
            snake.model->Rotate(0.1f, Axis::X);
            break;
        case GLFW_KEY_R:
            Reset(true);
            break;
        case GLFW_KEY_1:
            SetCamera(1);
            break;
        case GLFW_KEY_0:
            SetCamera(0);
            break;
        case GLFW_KEY_S:
            animate = !animate;
            break;
        case GLFW_KEY_T: // Simulating level up
            currentLevel++;
            Reset(false);
            break;
        case GLFW_KEY_J:
            currentScore += 1000;
            currentScoreFormatted = formatScore(currentScore);
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            currentLives++;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            currentLives--;
            break;
        }
        
    }
}

void BasicScene::ViewportSizeCallback(Viewport* _viewport)
{
    for (auto& cam: cameras)
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void BasicScene::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}

//void BasicScene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
//{
//    if (dragging) {
//        auto system = camera->GetRotation().transpose() * GetRotation();
//        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
//        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
//        if (pickedModel) {
//            pickedModel->SetTout(pickedToutAtPress);
//            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE)
//                pickedModel->TranslateInSystem(system,
//                                               {float(x - xAtPress) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
//            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
//                pickedModel->RotateInSystem(system, float(x - xAtPress) / moveCoeff, Axis::Z);
//            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE) {
//                pickedModel->RotateInSystem(system, float(x - xAtPress) / moveCoeff, Axis::Y);
//                pickedModel->RotateInSystem(system, float(y - yAtPress) / moveCoeff, Axis::X);
//            }
//        }
//        else {
//            // camera->SetTout(cameraToutAtPress);
//            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
//                root->TranslateInSystem(system, { -float(xAtPress - x) / moveCoeff / 10.0f, float(yAtPress - y) / moveCoeff / 10.0f, 0 });
//            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
//                root->RotateInSystem(system, float(x - xAtPress) / 180, Axis::Z);
//            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
//                root->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
//                root->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
//            }
//        }
//        xAtPress = x;
//        yAtPress = y;
//    }
//}
