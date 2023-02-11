#pragma once

#include "SceneWithImGui.h"
#include "GLFW/glfw3.h"
#include "Gameplay.h"


class BasicScene : public cg3d::SceneWithImGui
{
public:
    //explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    BasicScene(std::string name, cg3d::Display *display);

    void Init(float fov, int width, int height, float near, float far);

    void Update(const cg3d::Program &program, const Eigen::Matrix4f &proj, const Eigen::Matrix4f &view,
                const Eigen::Matrix4f &model) override;

    void BuildImGui() override;


private:
    // cameras
    std::vector<std::shared_ptr<cg3d::Camera>> cameras{3};
    cg3d::Viewport* viewport = nullptr;
    float FOV = 0, NEAR = 0, FAR = 0;
    int WIDTH = 0, HEIGHT = 0;
    void SetCamera(int index);

    void InitCameras();
    void ResetCameras();
    void SetCamerasView();

    void updateGameplay() override;

    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void ViewportSizeCallback(cg3d::Viewport* _viewport) override;
    void AddViewportCallback(cg3d::Viewport* _viewport) override;

};
