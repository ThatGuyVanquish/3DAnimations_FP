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
    for (int i = 0; i < cameras.size(); i++)
    {
        cameras[i] = Camera::Create("camera " + std::to_string(i), FOV, float(WIDTH) / float(HEIGHT), NEAR, FAR);
    }

    SetCamerasView();

    camera = cameras[0];
    gameplay.cyls[0].model->AddChild(cameras[1]);
    gameplay.cyls[0].model->AddChild(cameras[2]);

}

void BasicScene::ResetCameras()
{
    for (int i = 0; i < cameras.size(); i++)
    {
        cameras[i]->SetTout(Eigen::Affine3f::Identity());
        cameras[i]->SetTin(Eigen::Affine3f::Identity());
        cameras[i]->PropagateTransform();
    }
    SetCamerasView();
    SetCamera(0);
}

void BasicScene::SetCamerasView()
{
    Eigen::Matrix3f rotation;

    cameras[0]->Translate(30, Axis::Z);
    cameras[0]->Translate(15, Axis::Y);
    cameras[0]->Rotate((float)-M_PI_4/2.0, Axis::X);

    cameras[1]->Translate({0, 0, -2});

    rotation << 0.999981f, 0.00187451f, 0.0059422f, -0.00398497f, 0.925535f, 0.378641f, -0.00478994f,   -0.378658f,    0.925524f;
    cameras[2]->Translate({0, 4.88115, 10.5869});
    cameras[2]->Rotate(rotation);
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
    gameplay.imGuiOverlay.DeathScreen(gameplay.animate);
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
    gameplay.updateGameplay();
    if (gameplay.callResetCameras)
    {
        ResetCameras();
        gameplay.callResetCameras = false;
    }
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
//        case GLFW_KEY_Q:
//            gameplay.cyls[0].model->Rotate(0.1f, Axis::Z);
//            gameplay.cyls[1].model->Rotate(-0.1f, Axis::Z);
//            gameplay.snake.model->Rotate(-0.1f, Axis::Z);
//            break;
//        case GLFW_KEY_E:
//            gameplay.cyls[0].model->Rotate(-0.1f, Axis::Z);
//            gameplay.cyls[1].model->Rotate(0.1f, Axis::Z);
//            gameplay.snake.model->Rotate(0.1f, Axis::Z);
//            break;
        case GLFW_KEY_R:
            gameplay.Reset(true);
            if (gameplay.callResetCameras)
            {
                ResetCameras();
                gameplay.callResetCameras = false;
            }
            break;
        case GLFW_KEY_3:
            SetCamera(2);
            gameplay.head.model->showWireframe = true;
            break;
        case GLFW_KEY_1:
            SetCamera(1);
            gameplay.head.model->showWireframe = false;
            break;
        case GLFW_KEY_0:
            SetCamera(0);
            gameplay.head.model->showWireframe = true;
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
        case GLFW_KEY_P:
            std::cout << "camera[1] translate:" << cameras[1]->GetTout().translation() << std::endl;
            std::cout << "camera[1] rotation:" << cameras[1]->GetTout().rotation() << std::endl;


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
