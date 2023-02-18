#pragma once

#include "CollisionDetection.cpp"
#include "ImGuiOverlay.h"
#include <utility>
#include "IglMeshLoader.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"
#include <AABB.h>
#include "Movable.h"
#include "Skinning.h"
#include <thread>
#include "common.h"
#include "TextureCoordinates.h"
#include "Simplification.h"

using namespace cg3d;

class Gameplay
{

public:

    void Init();

    void InitMaterials();

    void InitSnake();

    void InitCoordsys();

    void generateViableEntities();

    entity_data initEntity(Entity ent, std::shared_ptr<cg3d::Material> material, bool visible = true);

    void randomizeTranlate(entity_data& entity);

    void spawnEntity(int index, std::vector<Entity> &viableEntities);

    void spawnEntities(int amount, std::vector<Entity>& viableEntities);

    void spawnExtras();

    void swapEntities(entity_data& entity, std::vector<entity_data> extras);

    void replaceEntity(entity_data& entity);

    void clearEntities();

    void InitLevel();

    void checkForCollision();

    //void checkTimedOutEntities();

    //void deleteEntityIfTimedOut(int index);

    void DeleteEntity(int index);

    void findAndDeleteEntity(entity_data& entity);

    void ResetSnake();

    void Reset(bool mainMenu);

    void UpdateScore(int score);

    void handleBonus();

    bool shouldLevelUp();

    void updateGameplay();

    void HandleEntityCollision(int index);

    Eigen::MatrixXd getTextureCoords(const char* filename);


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
    std::vector<entity_data> extraItems;
    std::vector<Entity> viableEnemies;
    std::vector<entity_data> extraEnemies;
    std::vector<Entity> viableBonuses;
    model_data head;
    model_data snake;
    model_data coordsys;

    enum class Bonus { SPEED_PLUS, SPEED_MINUS, LIFE, POINTS };

    std::vector<Gameplay::Bonus> bonusPercentage = { Bonus::LIFE, Bonus::LIFE,
        Bonus::POINTS, Bonus::POINTS, Bonus::POINTS, Bonus::POINTS, Bonus::POINTS,
        Bonus::POINTS, Bonus::POINTS, Bonus::POINTS, Bonus::POINTS, Bonus::POINTS,
        Bonus::SPEED_PLUS, Bonus::SPEED_PLUS, Bonus::SPEED_PLUS, Bonus::SPEED_PLUS,
        Bonus::SPEED_MINUS, Bonus::SPEED_MINUS, Bonus::SPEED_MINUS, Bonus::SPEED_MINUS };

    // materials
    std::shared_ptr<cg3d::Program> program;
    std::shared_ptr<cg3d::Program> snakeShader;
    std::shared_ptr<cg3d::Material> basicMaterial;
    std::shared_ptr<cg3d::Material> frameColor;
    std::shared_ptr<cg3d::Material> collisionColor;
    std::shared_ptr<cg3d::Material> snakeSkin;
    std::shared_ptr<cg3d::Program> itemShader;
    std::shared_ptr<cg3d::Material> itemMaterial;
    std::shared_ptr<cg3d::Program> enemyShader;
    std::shared_ptr<cg3d::Material> enemyMaterial;
    std::shared_ptr<cg3d::Program> bonusShader;
    std::shared_ptr<cg3d::Material> bonusMaterial;

    // skinning
    Skinning snakeSkinning;
    bool useSnake = true;
    bool showCyls = false;

    // animation
    float slerpFactor = 0.9f;
    float prev_slerp;
    Eigen::Vector3f velocityVec = { 0, 0, -0.05f };
    time_t timeFromLastWASDQE = time(nullptr);
    Eigen::Vector3f currPos;

    // texture
    std::vector<Eigen::MatrixXd> uv_vec;
    int curr_uv = 0;


};