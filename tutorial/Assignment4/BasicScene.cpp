#include "BasicScene.h"
#include "stb_image.h"
#include <filesystem>

using namespace cg3d;

BasicScene::BasicScene(std::string name, cg3d::Display* display) : SceneWithImGui(std::move(name), display)
{
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
    /*
        Initiate 3 second timer then set animate to true and start scoreboard timer
    */
    float width = 500.0f, height = 500.0f;
    ImGui::Begin("StartTimer", mainMenuToggle, MENU_FLAGS);
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ShowLargeText("SHEEEEESH");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.0f, 0.0f, 1.0f));
    float originaFontlScale = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 2;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text("3");
    ImGui::PopFont();
    ImGui::GetFont()->Scale /= 2;
    ImGui::PopStyleColor();
    ImGui::End();
}

void BasicScene::Scoreboard()
{
    float width = 1920.0, height = 100.0;
    ImGui::CreateContext();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.12f));
    ImGui::Begin("Scoreboard", scoreboardToggle,(MENU_FLAGS - ImGuiWindowFlags_NoBackground));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor();
    ImGui::SameLine(200.0f, 3.0f);
    ImGui::SetCursorPos(ImVec2(120.0f, 30.0f));
    float old_size = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 3;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text("SCORE:");
    ImGui::SameLine(0.0f, 50.0f);
    if (currentScoreFormatted == nullptr)
        formatScore();
    ImGui::Text(currentScoreFormatted);
    ImGui::SetCursorPos(ImVec2(1500.0f, 30.0f));
    ImGui::Text("TIMER:");
    ImGui::SameLine(0.0f, 50.0f);
    ImGui::Text("30:00");
    ImGui::GetFont()->Scale = old_size;
    ImGui::PopFont();
    ImGui::End();
}

char* BasicScene::getResource(const char* fileName)
{
    std::filesystem::path cwd = std::filesystem::current_path() / "..\\..\\..\\tutorial\\Assignment4\\resources";
    std::filesystem::path filePath = cwd / fileName;
    std::string filePathString = filePath.string();
    return strcpy(new char[filePathString.length() + 1], filePathString.c_str());
}

void BasicScene::MainMenu()
{
    ImGui::CreateContext();
    ImGui::Begin("Main Menu", mainMenuToggle, MENU_FLAGS);
    
    int width, height, nrChannels;
    //std::filesystem::path cwd = std::filesystem::current_path() / "..\\..\\..\\tutorial\\Assignment4\\resources";
    //std::filesystem::path imagePath = cwd / "mainmenu_bg.png";
    //std::string imagePathString = imagePath.string();
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
    ImGui::SetWindowPos("Main Menu", ImVec2(675, 275), ImGuiCond_Always);
    ImGui::SetItemAllowOverlap();
    ImGui::SetWindowSize("Main Menu", ImVec2(width + 20, height + 20), ImGuiCond_Always);
    bool* pOpen2 = nullptr;
    //ImGui::Text("SNAAAAAAAKE");
    ImGui::SetCursorPos(ImVec2(105, 185));
    ImGui::SetWindowFontScale(1.3f);
    if (ImGui::Button("START GAME"))
    {
        showMainMenu = false;
        gaming = true;
        //startTimer();
    }
    ImGui::End();
}

void BasicScene::BuildImGui()
{
    /*if (!initializedFonts)
        initFonts();*/
    if (showMainMenu)
        MainMenu();
    if (gaming)
        Scoreboard();
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
    cyls[0].model->AddChild(cameras[1]);
    cameras[1]->Translate(0.5f, Axis::Y);
}

void BasicScene::InitSnake()
{
    auto cylMesh = IglLoader::MeshFromFiles("cyl_igl","data/xCylinder.obj");
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("material", program); // empty material
    auto green{ std::make_shared<Material>("green", program, true)};
    green->AddTexture(0, "textures/grass.bmp", 2);
    red = { std::make_shared<Material>("red", program) };
    red->AddTexture(0, "textures/box0.bmp", 2);

    float scaleFactor = 1.0f;
    igl::AABB<Eigen::MatrixXd, 3> aabb = InitAABB(cylMesh);
    for(int i = 0; i < numOfCyls; i++)
    {
        auto cylModel = Model::Create("Cyl " + std::to_string(i), cylMesh, material);
        cyls.push_back({cylModel, scaleFactor, aabb});
        auto collisionBox = cg3d::Model::Create("Bounding box 0", CollisionDetection::meshifyBoundingBox(aabb.m_box),green);
        cyls[i].model->AddChild(collisionBox);
        collisionBox->showFaces = false;
        collisionBox->showWireframe = true;
        cyls[i].model->showFaces = false;

        if (i == 0) // first axis and cylinder depend on scene's root
        {
            root->AddChild(cyls[0].model);
            cyls[0].model->Translate({ 0.8f, 0, 0 });
            cyls[0].model->SetCenter(Eigen::Vector3f(-0.8f, 0, 0));
        }
        else
        {
            cyls[i - 1].model->AddChild(cyls[i].model);
            //cyls[i].model->Scale(1, Axis::X);
            cyls[i].model->Translate(1.6f, Axis::X);
            cyls[i].model->SetCenter(Eigen::Vector3f(-0.8f, 0, 0));
        }
    }
//    auto phongShader = std::make_shared <cg3d::Program>("shaders/phongShader");
//    auto material2 = std::make_shared<cg3d::Material>("material2", phongShader);
    auto camelHeadMesh = IglLoader::MeshFromFiles("camelhead", "data/camelhead.off");
    auto camelHeadModel = Model::Create("Camelhead", camelHeadMesh, material);
    aabb = InitAABB(camelHeadMesh);
    camelHead = {camelHeadModel, 1.5f, aabb};
    auto collisionBox = cg3d::Model::Create("Bounding box 0", CollisionDetection::meshifyBoundingBox(aabb.m_box),green);
    camelHead.model->AddChild(collisionBox);
    collisionBox->showFaces = false;
    collisionBox->showWireframe = true;
    collisionBox->Scale(camelHead.scaleFactor);
    camelHeadModel->Scale(camelHead.scaleFactor);
    camelHeadModel->Rotate(-M_PI_2, Axis::Y);
    camelHeadModel->Translate(-1.6f, Axis::X);
    cyls[0].model->AddChild(camelHeadModel);
}

igl::AABB<Eigen::MatrixXd, 3> BasicScene::InitAABB(std::shared_ptr<Mesh> mesh)
{
    Eigen::MatrixXd V = mesh->data[0].vertices;
    Eigen::MatrixXi F = mesh->data[0].faces;
    igl::AABB<Eigen::MatrixXd, 3> axisAligned;
    axisAligned.init(V, F);
    return axisAligned;
}

void BasicScene::initFonts()
{
    initializedFonts = true;
    ImGuiIO io = ImGui::GetIO();
    char* arial = getResource("ARIAL.TTF");
    io.Fonts->AddFontFromFileTTF(arial, 16.0f);
    io.Fonts->AddFontFromFileTTF(arial, 32.0f);
    io.Fonts->AddFontFromFileTTF(arial, 64.0f);
}

void BasicScene::ShowSmallText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::Text(text);
    ImGui::PopFont();
}

void BasicScene::ShowMediumText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Text(text);
    ImGui::PopFont();
}

void BasicScene::ShowLargeText(const char* text)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
    ImGui::Text(text);
    ImGui::PopFont();
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

    // Creation of axis mesh
    Eigen::MatrixXd vertices(6, 3);
    vertices << -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1;
    Eigen::MatrixXi faces(3, 2);
    faces << 0, 1, 2, 3, 4, 5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6, 3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6, 2);
    coordsys = std::make_shared<Mesh>("coordsys", vertices, faces, vertexNormals, textureCoords);
    auto axis = Model::Create("Axis", coordsys, material);
    axis->mode = 1;
    axis->Scale(20);
    AddChild(axis);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
    program.SetUniform1f("specular_exponent", 5.0);
    program.SetUniform4f("light_position", 0.0, 15.0, 0.0, 1.0);
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
        Eigen::AlignedBox3d box0, box1;
        if (CollisionDetection::intersects(
                camelHead.scaleFactor,
                camelHead.aabb,
                camelHead.model->GetAggregatedTransform(),
                cyls[i].scaleFactor,
                cyls[i].aabb,
                cyls[i].model->GetAggregatedTransform(),
                box0, box1
        ))
        {
            animate = false;
        }
    }

    // search for collision between cylinders
    for (int i = 0; i < cyls.size(); i++) {
        for (int j = i+2; j < cyls.size(); j++) {
            Eigen::AlignedBox3d box0, box1;
            if (CollisionDetection::intersects(
                    cyls[i].scaleFactor,
                    cyls[i].aabb,
                    cyls[i].model->GetAggregatedTransform(),
                    cyls[j].scaleFactor,
                    cyls[j].aabb,
                    cyls[j].model->GetAggregatedTransform(),
                    box0, box1
            ))
            {
                animate = false;
                auto m0 = cg3d::Model::Create(
                        "cb0",
                        cg3d::Mesh::Cube(),
                        red
                );
                auto m1 = cg3d::Model::Create(
                        "cb1",
                        cg3d::Mesh::Cube(),
                        red
                );
                std::vector<std::shared_ptr<cg3d::Mesh>> test0, test1;
                test0.push_back(CollisionDetection::meshifyBoundingBox(box0));
                test1.push_back(CollisionDetection::meshifyBoundingBox(box1));
                m0->SetMeshList(test0);
                m1->SetMeshList(test1);
                m0->showFaces = false;
                m1->showFaces = false;
                m0->showWireframe = true;
                m1->showWireframe = true;
                m0->isHidden = false;
                m1->isHidden = false;
                cyls[i].model->AddChild(m0);
                cyls[j].model->AddChild(m1);

            }
        }
    }

}

Eigen::Vector3f dir = { 1,0,0 };

void BasicScene::KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
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
            break;
        case GLFW_KEY_RIGHT:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Y);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Y);
            cyls[0].model->Rotate(-0.1f, Axis::Y);
            cyls[1].model->Rotate(0.1f, Axis::Y);
            break;
        case GLFW_KEY_UP:
//            cyls[0].model->RotateInSystem(system, 0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, -0.1f, Axis::Z);
            cyls[0].model->Rotate(-0.1f, Axis::Z);
            cyls[1].model->Rotate(0.1f, Axis::Z);
            break;
        case GLFW_KEY_DOWN:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Z);
            cyls[0].model->Rotate(0.1f, Axis::Z);
            cyls[1].model->Rotate(-0.1f, Axis::Z);
            break;
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
            startTimer();
            break;
        }
        
    }
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
