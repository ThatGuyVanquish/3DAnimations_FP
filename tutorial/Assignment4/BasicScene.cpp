#include "BasicScene.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create("camera", fov, float(width) / float(height), near, far);
    AddChild(root = Movable::Create("root")); // a common invisible parent object for all the shapes

    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material = std::make_shared<Material>("material", program); // empty material
    cube = Model::Create("cube", Mesh::Cube(), material);
    //AddChild(cube);
    auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake2.obj");
    auto snake = Model::Create("SSSSSSSSSSSSSSSSSSSNAKE", snakeMesh, material);
    AddChild(snake);
    snake->Scale(0.5);
    camera->Translate(15, Axis::Z);
    //cube->Scale(3);
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);

    cube->Rotate(0.01f, Axis::XYZ);
}
