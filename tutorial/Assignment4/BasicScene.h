#pragma once

#include "Scene.h"
#include "SceneWithImGui.h"
#include "CamModel.h"
#include <utility>
#include "IglMeshLoader.h"
#include "imgui.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"

class BasicScene : public cg3d::SceneWithImGui
{
public:
    //explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    BasicScene(std::string name, cg3d::Display* display);
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void BuildImGui() override;
    bool animate = false;
private:
    
    std::shared_ptr<cg3d::Movable> root;
    std::vector<std::shared_ptr<cg3d::Camera>> cameras{ 2 };
    // camera[0] = top down view
    // camera[1] = snake view

    std::vector<std::shared_ptr<cg3d::Model>> cyls;
    void InitSnake(int num = 5);
    void InitCameras(float fov, int width, int height, float near, float far);
    void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
};
