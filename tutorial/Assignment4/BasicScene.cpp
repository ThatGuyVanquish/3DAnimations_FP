#include "BasicScene.h"

using namespace cg3d;

BasicScene::BasicScene(std::string name, cg3d::Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

bool* pOpen = nullptr;

void BasicScene::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Main Menu", pOpen, flags);
    //ImGui::SetWindowPos("Main Menu", ImVec2(800, 450), ImGuiCond_Always); // set to never when started gameplay
    ImGui::SetWindowSize("Main Menu", ImVec2(200, 250), ImGuiCond_Always);
    ImGui::Text("I'M A SNAKEEEE. I'M A SNEAKY SNEAKY SNAKEEE");
    bool* pOpen2 = nullptr;
    if (ImGui::Button("BLYAT SUCCA"))
    {
        //ImGui::Begin("Test", pOpen2, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        //ImGui::SetWindowSize("Test", ImVec2(200, 250), ImGuiCond_Always);
        //ImGui::Text("I'M A SNAKEEEE. I'M A SNEAKY SNEAKY SNAKEEE");
    }
    /*ImGui::SetWindowCollapsed("Main Menu", pOpen, ImGuiCond_Always);*/
    ImGui::End();
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
            //ImGui::Begin("Main Menu", pOpen, ImGuiCond_Always);
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
        }
    }
}
