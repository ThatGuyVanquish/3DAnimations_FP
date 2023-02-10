#include "BasicScene.h"
#include "stb_image.h"
#include <chrono>
#include <filesystem>
#include <random>

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
    initObjects();
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

    //auto snakeShader = std::make_shared<Program>("shaders/overlay");
    //auto snakeSkin = std::make_shared<Material>("snakeSkin", snakeShader);
    //snakeSkin->AddTexture(0, "textures/snake1.png", 2);
    //auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake2.obj");
    //auto snake = Model::Create("SSSSSSSSSSSSSSSSSSSNAKE", snakeMesh, snakeSkin);
    //AddChild(snake);
    //snake->Scale(0.5);
    camera->Translate(30, Axis::Z);
    camera->Translate(15, Axis::Y);
    camera->Rotate(-M_PI_4/2.0, Axis::X);
    //cube->Scale(3);

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
    generateViableEntities();
    spawnEntity(0);
    //auto test = IglLoader::MeshFromFiles("test", "data/Sword01.off");
    //auto testmodel = Model::Create("apple", test, basicMaterial);
    //testmodel->Translate({ 0.0, 0.0, -5.0 });
    ////testmodel->RotateByDegree(M_PI_2, Axis::Y);
    //testmodel->Scale(0.03f);
    //testmodel->showFaces = true;
    //testmodel->showWireframe = true;
    //testmodel->showTextures = true;
    //root->AddChild(testmodel);

    //auto test2 = IglLoader::MeshFromFiles("test", "data/Apple.off");
    //auto testmodel2 = Model::Create("apple", test2, basicMaterial);
    //testmodel2->Translate({ 0.0, 0.0, -20.0 });
    //testmodel->RotateByDegree(-M_PI_2, Axis::Z);
    //testmodel2->Scale(0.05f);
    //testmodel2->showFaces = true;
    //testmodel2->showWireframe = true;
    //testmodel2->showTextures = true;
    //root->AddChild(testmodel2);

    //auto test2 = IglLoader::MeshFromFiles("test", "data/bumpy-cube.obj");
    //auto testmodel2 = Model::Create("apple", test2, basicMaterial);
    //testmodel2->Translate({ 0.0, 0.0, -30.0f });
    ////testmodel2->RotateByDegree(M_PI_2, Axis::Y);
    //testmodel2->Scale(5.0f);
    //testmodel2->showFaces = true;
    //testmodel2->showWireframe = true;
    //testmodel2->showTextures = true;
    //root->AddChild(testmodel2);

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
    cameras[1]->Rotate(M_PI_2, Axis::Y);
}

void BasicScene::InitSnake()
{
    //init cylinders
    auto cylMesh = IglLoader::MeshFromFiles("cyl_igl","data/zCylinder.obj");
    float scaleFactor = 1.0f;
    igl::AABB<Eigen::MatrixXd, 3> cyl_aabb = InitAABB(cylMesh);
    for(int i = 0; i < numOfCyls; i++)
    {
        auto cylModel = Model::Create("Cyl " + std::to_string(i), cylMesh, basicMaterial);
        cyls.push_back({cylModel, scaleFactor, cyl_aabb});
        InitCollisionModels(cyls[i]);
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
    igl::AABB<Eigen::MatrixXd, 3> head_aabb = InitAABB(headMesh);
    head = {headModel, 1.0f, head_aabb};
    InitCollisionModels(head);
    headModel->Scale(head.scaleFactor);
    headModel->Rotate(-M_PI, Axis::Y);
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
    igl::AABB<Eigen::MatrixXd, 3> snake_aabb = InitAABB(snakeMesh);
    snake = {snakeModel, 16.0f, snake_aabb};
    snakeModel->Translate(1.6*(numOfCyls/2)-0.8, Axis::Z);
    snakeModel->Scale(16.0f, Axis::Z);
    snakeModel->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
//    InitCollisionModels(head); // should we fix cylinder collision or use snake mesh? (problem with scaling in only one axis)
//    cyls[0].model->AddChild(snakeModel);

}

void BasicScene::initObjects()
{
    auto bunnyMesh = IglLoader::MeshFromFiles("bunny", "data/bunny.off");
    auto bunnyModel = Model::Create("bunny", bunnyMesh, basicMaterial);
    igl::AABB<Eigen::MatrixXd, 3> aabb = InitAABB(bunnyMesh);
    objects.push_back({bunnyModel, 3.0f, aabb});
    InitCollisionModels(objects[0]);
    root->AddChild(bunnyModel);
    bunnyModel->Translate({0.0, 0.0, -5.0});
    bunnyModel->Scale(objects[0].scaleFactor);
    bunnyModel->showFaces = false;
    bunnyModel->showWireframe = true;
}

void BasicScene::initEntity(Entity ent, std::shared_ptr<cg3d::Material> material)
{
    auto mesh = cg3d::IglLoader::MeshFromFiles("Entity_" + entityTypeToString(ent.type) + ent.name, ent.pathToMesh);
    auto model = cg3d::Model::Create("Entity_" + entityTypeToString(ent.type) + ent.name, mesh, material);
    igl::AABB<Eigen::MatrixXd, 3> aabb = InitAABB(mesh);
    model_data currentModel = {model, ent.scale, aabb};
    //objects.push_back(currentModel);
    InitCollisionModels(currentModel);
    root->AddChild(model);
    model->Translate({ 0.0, 0.0, -5.0 });
    model->Scale(ent.scale);
    model->showFaces = false;
    model->showWireframe = true;
    entity_data currentEntity = { currentModel, std::chrono::high_resolution_clock::now(), 
        {ent.name, ent.pathToMesh, ent.scale ,ent.type, ent.points, ent.lifeTime / currentLevel} };
    entities.push_back(currentEntity);
}

static int getRandomNumberInRange(int low, int high)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(low, high); // define the range
    return distr(gen);
}

void BasicScene::spawnEntity(int index)
{
    if (index == -1)
        index = getRandomNumberInRange(0, entities.size());
    int x_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int z_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int y_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    initEntity(viableEntities[index], basicMaterial);
    entities[entities.size() - 1].model.model->Translate({ (float)x_value, (float)y_value, (float)z_value });
}

void BasicScene::generateViableEntities()
{
    viableEntities.push_back({ "Bunny", "data/bunny.off", 6.0f,EntityType::ITEM, 1000, 100 });
    viableEntities.push_back({ "Cheburashka", "data/cheburashka.off", 1.0f, EntityType::ITEM, 500, 80 });
    viableEntities.push_back({ "Cow", "data/cow.off", 2.0f, EntityType::ITEM, 500, 80 });
    viableEntities.push_back({ "Screwdriver", "data/screwdriver.off", 10.0f, EntityType::ENEMY, -1000, 100 });
    viableEntities.push_back({ "Knight", "data/decimated-knight.off", 2.0f, EntityType::ENEMY, -500, 80 });
    viableEntities.push_back({ "Torus", "data/torus.obj", 0.3f, EntityType::BONUS, 0, 50 });
    viableEntities.push_back({ "Sword", "data/Sword01.off", 0.05f, EntityType::ENEMY, -1000, 100 });
    viableEntities.push_back({ "Sword", "data/Apple.off", 0.05f, EntityType::ITEM, 500, 100 });
    // maybe add magnet bonus
}

void BasicScene::InitLevel()
{
    int enemies, items, bonuses;
    switch (currentLevel) 
    {
        case 1:
            enemies = 3;
            items = 10;
            bonuses = 2;
            break;

    }
}

/**
 * collision detection methods:
 * 1. InitAABB - AABB init method for a given mesh object
 * 2. InitCollisionModels
 * 3. checkForCollision
 */
igl::AABB<Eigen::MatrixXd, 3> BasicScene::InitAABB(std::shared_ptr<Mesh> mesh)
{
    Eigen::MatrixXd V = mesh->data[0].vertices;
    Eigen::MatrixXi F = mesh->data[0].faces;
    igl::AABB<Eigen::MatrixXd, 3> axisAligned;
    axisAligned.init(V, F);
    return axisAligned;
}

void BasicScene::InitCollisionModels(model_data &modelData) {
    auto collisionFrame = cg3d::Model::Create(
            "collisionFrame "+modelData.model->name,
            CollisionDetection::meshifyBoundingBox(modelData.aabb.m_box),
            green
    );
    collisionFrame->showFaces = false;
    collisionFrame->showWireframe = true;
    collisionFrame->Scale(modelData.scaleFactor);
    modelData.model->AddChild(collisionFrame);
    modelData.collisionFrame = collisionFrame;

    auto collisionBox = cg3d::Model::Create(
            "collisionBox "+modelData.model->name,
            cg3d::Mesh::Cube(),
            red
    );
    collisionBox->showFaces = false;
    collisionBox->showWireframe = false;
    collisionBox->isHidden = true;
    collisionBox->isPickable = false;
    modelData.model->AddChild(collisionBox);
    modelData.collisionBox = collisionBox;
}

void BasicScene::SetCollisionBox(model_data &modelData, Eigen::AlignedBox3d box)
{
    std::vector<std::shared_ptr<cg3d::Mesh>> meshVec;
    meshVec.push_back(CollisionDetection::meshifyBoundingBox(box));
    modelData.collisionBox->SetMeshList(meshVec);
    modelData.collisionBox->showWireframe = true;
    modelData.collisionBox->isHidden = false;
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
            SetCollisionBox(head, box_camel);
            SetCollisionBox(cyls[i], box_i);
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
            SetCollisionBox(head, box_camel);
            SetCollisionBox(objects[i], box_i);
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
                SetCollisionBox(cyls[i], box_i);
                SetCollisionBox(cyls[j], box_j);
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
                SetCollisionBox(cyls[i], box_i);
                SetCollisionBox(objects[j], box_j);
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
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
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
        currentScoreFormatted = formatScore(0);
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

void BasicScene::BuildImGui()
{
    if (showMainMenu)
        MainMenu();
    if (gaming)
        Scoreboard();
    if (!started)
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
            showMainMenu = true;
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
        case GLFW_KEY_J:
            currentScore += 1000;
            currentScoreFormatted = formatScore(currentScore);
            break;
        case GLFW_KEY_T:
            //startTimer();
            started = false;
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
