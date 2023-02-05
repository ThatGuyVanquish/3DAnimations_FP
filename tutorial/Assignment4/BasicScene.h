#pragma once

#include "Scene.h"
#include "SceneWithImGui.h"
#include "CamModel.h"
#include <utility>
#include "IglMeshLoader.h"
#include "imgui.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"
#include <AABB.h>
#include <map>
#include "CollisionDetection.cpp"

//#include <glad/glad.h>

typedef struct
{
    std::shared_ptr<cg3d::Model> model;
    float scaleFactor;
    igl::AABB<Eigen::MatrixXd, 3> aabb;
} model_data;

class BasicScene : public cg3d::SceneWithImGui {
public:
    //explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    BasicScene(std::string name, cg3d::Display *display);

    void Init(float fov, int width, int height, float near, float far);

    void Update(const cg3d::Program &program, const Eigen::Matrix4f &proj, const Eigen::Matrix4f &view,
                const Eigen::Matrix4f &model) override;

    void BuildImGui() override;

    void MainMenu();

    void Scoreboard();

    void startTimer();

private:
    void formatScore();

    char *getResource(const char *fileName);

    bool initializedFonts = false;

    void initFonts();

    void ShowSmallText(const char *text);

    void ShowMediumText(const char *text);

    void ShowLargeText(const char *text);

    bool gaming = false;
    bool started = true;

    bool *mainMenuToggle = nullptr;
    bool showMainMenu = true;

    int currentScore = 0;
    char *currentScoreFormatted = nullptr;
    bool *scoreboardToggle = nullptr;

    bool *startTimerToggle = nullptr;

    std::shared_ptr<cg3d::Movable> root;
    std::vector<std::shared_ptr<cg3d::Camera>> cameras{2};
    std::shared_ptr<cg3d::Mesh> coordsys;
    std::vector<model_data> cyls;
    model_data camelHead;

    // camera[0] = top down view
    // camera[1] = snake view
    static const int MENU_FLAGS =
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

    void InitSnake();

    void InitCameras(float fov, int width, int height, float near, float far);
//    void CursorPosCallback(cg3d::Viewport* viewport, int xNew, int yNew, bool dragging, int* buttonState) override;

    void KeyCallback(cg3d::Viewport *viewport, int x, int y, int key, int scancode, int action, int mods) override;


    // collision detection
    igl::AABB<Eigen::MatrixXd, 3> InitAABB(std::shared_ptr<cg3d::Mesh> mesh);
    void checkForCollision() override;
    std::shared_ptr<cg3d::Material> red;

};
