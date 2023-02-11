#pragma once

#include "CollisionDetection.cpp"
#include "ImGuiOverlay.h"
#include <utility>
#include "IglMeshLoader.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"
#include <AABB.h>
#include "Movable.h"

// header for common structures like model_data
#include "common.h"

using namespace cg3d;



class Gameplay
{

public:

    void Init();

    void InitMaterials();

    void InitSnake();

    void InitCoordsys();

    void generateViableEntities();

    void initEntity(Entity ent, std::shared_ptr<cg3d::Material> material);

    void spawnEntity(int index, std::vector<Entity> &viableEntities);

    void spawnEntities(int amount, std::vector<Entity>& viableEntities);

    void clearEntities();

    void InitLevel();

    void checkForCollision();

    void checkTimedOutEntities();

    void deleteEntityIfTimedOut(int index);

    void DeleteEntity(int index);

    void ResetSnake();

    void Reset(bool mainMenu);

    void UpdateScore(int score);

    bool shouldLevelUp();

    void updateGameplay();

    void HandleEntityCollision(int index);


    ImGuiOverlay imGuiOverlay;
    const int MAP_SIZE = 50;
    int numOfCyls = 16;
    bool animate, callResetCameras;

    int currentEnemies = 0, currentItems = 0, currentBonuses = 0;

    // models
    std::shared_ptr<cg3d::Movable> root;
    std::vector<model_data> cyls;
    std::vector<entity_data> entities;
    std::vector<Entity> viableItems;
    std::vector<Entity> viableEnemies;
    std::vector<Entity> viableBonuses;
    model_data head;
    model_data snake;
    model_data coordsys;

    // materials
    std::shared_ptr<cg3d::Program> program;
    std::shared_ptr<cg3d::Program> snakeShader;
    std::shared_ptr<cg3d::Material> basicMaterial;
    std::shared_ptr<cg3d::Material> frameColor;
    std::shared_ptr<cg3d::Material> collisionColor;
    std::shared_ptr<cg3d::Material> snakeSkin;
};
