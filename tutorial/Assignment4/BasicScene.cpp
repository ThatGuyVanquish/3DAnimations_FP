#pragma once

#include "BasicScene.h"


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

    AddChild(gameplay.root = Movable::Create("root")); // a common invisible parent object for all the shapes

    FOV = fov; WIDTH = width; HEIGHT = height; NEAR = near; FAR = far;

    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    gameplay.Init();
    InitCameras();

    AddChild(gameplay.coordsys.model);

}

void BasicScene::InitCameras()
{
    for (int i = 0; i < 2; i++)
    {
        cameras[i] = Camera::Create("camera " + std::to_string(i), FOV, float(WIDTH) / float(HEIGHT), NEAR, FAR);
    }
    //set up camera location
    camera = cameras[0];
    //maybe place snake then have camera[1] relative to it based on being a child of snake's root
    gameplay.cyls[0].model->AddChild(cameras[1]);
    cameras[1]->Translate(0.5f, Axis::Y);
    cameras[1]->Translate(0.5, Axis::X);
    cameras[1]->Rotate((float)M_PI_2, Axis::Y);

    camera->Translate(30, Axis::Z);
    camera->Translate(15, Axis::Y);
    camera->Rotate((float)-M_PI_4/2.0, Axis::X);
}

void BasicScene::SetCamera(int index)
{
    camera = cameras[index];
    viewport->camera = camera;
}

void BasicScene::BuildImGui()
{
    gameplay.imGuiOverlay.MainMenu(gameplay.animate);
    gameplay.imGuiOverlay.Scoreboard(gameplay.animate);
    gameplay.imGuiOverlay.startTimer(gameplay.animate);
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

void BasicScene::updateGameplay()
{
    bool callInitCameras;
    gameplay.updateGameplay(callInitCameras);
    if (callInitCameras) InitCameras();
}

void BasicScene::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
//    auto system = camera->GetRotation().transpose();
    auto system = gameplay.cyls[0].model->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
//            cyls[0].model->RotateInSystem(system, 0.1f, Axis::Y);
//            cyls[1].model->RotateInSystem(system, -0.1f, Axis::Y);
                gameplay.cyls[0].model->Rotate(0.1f, Axis::Y);
                gameplay.cyls[1].model->Rotate(-0.1f, Axis::Y);
                gameplay.snake.model->Rotate(-0.1f, Axis::Y);
            break;
        case GLFW_KEY_RIGHT:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Y);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Y);
                gameplay.cyls[0].model->Rotate(-0.1f, Axis::Y);
                gameplay.cyls[1].model->Rotate(0.1f, Axis::Y);
                gameplay.snake.model->Rotate(0.1f, Axis::Y);
            break;
        case GLFW_KEY_UP:
//            cyls[0].model->RotateInSystem(system, 0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, -0.1f, Axis::Z);
                gameplay.cyls[0].model->Rotate(0.1f, Axis::X);
                gameplay.cyls[1].model->Rotate(-0.1f, Axis::X);
                gameplay.snake.model->Rotate(-0.1f, Axis::X);
            break;
        case GLFW_KEY_DOWN:
//            cyls[0].model->RotateInSystem(system, -0.1f, Axis::Z);
//            cyls[1].model->RotateInSystem(system, 0.1f, Axis::Z);
                gameplay.cyls[0].model->Rotate(-0.1f, Axis::X);
                gameplay.cyls[1].model->Rotate(0.1f, Axis::X);
                gameplay.snake.model->Rotate(0.1f, Axis::X);
            break;
        case GLFW_KEY_R:
            gameplay.Reset(true);
            break;
        case GLFW_KEY_1:
            SetCamera(1);
            break;
        case GLFW_KEY_0:
            SetCamera(0);
            break;
        case GLFW_KEY_S:
            gameplay.animate = !gameplay.animate;
            break;
        case GLFW_KEY_T: // Simulating level up
            gameplay.imGuiOverlay.currentLevel++;
            gameplay.Reset(false);
            break;
        case GLFW_KEY_J:
            gameplay.UpdateScore(1000);
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            gameplay.imGuiOverlay.currentLives++;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            gameplay.imGuiOverlay.currentLives--;
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
