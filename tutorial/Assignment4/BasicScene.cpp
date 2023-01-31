#include "BasicScene.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create("camera", fov, float(width) / float(height), near, far);
    AddChild(root = Movable::Create("root")); // a common invisible parent object for all the shapes

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
    cube = Model::Create("cube", Mesh::Cube(), material);
    //AddChild(cube);
    auto snakeShader = std::make_shared<Program>("shaders/overlay");
    auto snakeSkin = std::make_shared<Material>("snakeSkin", snakeShader);
    snakeSkin->AddTexture(0, "textures/snake1.png", 2);
    auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake2.obj");
    auto snake = Model::Create("SSSSSSSSSSSSSSSSSSSNAKE", snakeMesh, snakeSkin);
    std::cout << "HYUUUUUUUUA" << snake->GetMesh(0)->data[0].textureCoords.rows()
        << "\nV SIZE IS " << snake->GetMesh(0)->data[0].vertices.rows() << std::endl;
    Eigen::Matrix2f test(snake->GetMesh(0)->data[0].vertices.rows());
    for (int i = 0; i < snake->GetMesh(0)->data[0].vertices.rows(); i++) {
        test(i, 0) = 0;
        test(i, 1) = 1;
    }
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
