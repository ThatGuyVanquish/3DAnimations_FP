#pragma once

#include "../engine/common.h"
#include "CollisionDetection.cpp"


static void generateViableEntities(std::vector<Entity>& viableEntities)
{
    viableEntities.push_back({ "Bunny", "data/bunny.off", 6.0f,EntityType::ITEM, 1000, 100 });
    viableEntities.push_back({ "Cheburashka", "data/cheburashka.off", 1.0f, EntityType::ITEM, 500, 80 });
    viableEntities.push_back({ "Cow", "data/cow.off", 2.0f, EntityType::ITEM, 500, 80 });
    viableEntities.push_back({ "Screwdriver", "data/screwdriver.off", 10.0f, EntityType::ENEMY, -1000, 100 });
    viableEntities.push_back({ "Knight", "data/decimated-knight.off", 2.0f, EntityType::ENEMY, -500, 80 });
    viableEntities.push_back({ "Torus", "data/torus.obj", 0.3f, EntityType::BONUS, 0, 50 });
    viableEntities.push_back({ "Sword", "data/Sword01.off", 0.05f, EntityType::ENEMY, -1000, 100 });
    viableEntities.push_back({ "Sword", "data/Apple.off", 0.05f, EntityType::ITEM, 500, 100 });
    // maybe add magnet bonus
}

static void initEntity(Entity ent, std::shared_ptr<cg3d::Material> material,
    std::vector<entity_data>& entities, 
    std::shared_ptr<cg3d::Material> &frameColor,
    std::shared_ptr<cg3d::Material> &collisionColor,
    std::shared_ptr<cg3d::Movable> &root, int currentLevel)
{
    auto mesh = cg3d::IglLoader::MeshFromFiles("Entity_" + entityTypeToString(ent.type) + ent.name, ent.pathToMesh);
    auto model = cg3d::Model::Create("Entity_" + entityTypeToString(ent.type) + ent.name, mesh, material);
    igl::AABB<Eigen::MatrixXd, 3> aabb = CollisionDetection::InitAABB(mesh);
    model_data currentModel = { model, ent.scale, aabb };
    //objects.push_back(currentModel);
    CollisionDetection::InitCollisionModels(currentModel, frameColor, collisionColor);
    root->AddChild(model);
    model->Translate({ 0.0, 0.0, -5.0 });
    model->Scale(ent.scale);
    model->showFaces = false;
    model->showWireframe = true;
    entity_data currentEntity = { currentModel, time(nullptr),
        {ent.name, ent.pathToMesh, ent.scale ,ent.type, ent.points, ent.lifeTime / currentLevel} };
    entities.push_back(currentEntity);
}

static void spawnEntity(int index, std::vector<Entity>& viableEntities, 
    std::vector<entity_data>& entities, const int MAP_SIZE, 
    std::shared_ptr<cg3d::Material> &basicMaterial,
    std::shared_ptr<cg3d::Material> &frameColor,
    std::shared_ptr<cg3d::Material> &collisionColor,
    std::shared_ptr<cg3d::Movable> &root, int currentLevel
    )
{
    if (index == -1)
        index = getRandomNumberInRange(0, entities.size());
    int x_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int z_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int y_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    initEntity(viableEntities[index], basicMaterial, entities, frameColor, collisionColor, root, currentLevel);
    entities[entities.size() - 1].modelData.model->Translate({ (float)x_value, (float)y_value, (float)z_value });
}

static void spawnEntities(int index, int amount,
    std::vector<Entity>& viableEntities,
    std::vector<entity_data>& entities, const int MAP_SIZE,
    std::shared_ptr<cg3d::Material> &basicMaterial,
    std::shared_ptr<cg3d::Material> &frameColor,
    std::shared_ptr<cg3d::Material> &collisionColor,
    std::shared_ptr<cg3d::Movable> &root, int currentLevel)
{
    for (; amount > 0; amount--)
        spawnEntity(index, viableEntities, entities, MAP_SIZE, basicMaterial, frameColor, collisionColor,
            root, currentLevel);
}


static void InitLevel(
    std::vector<Entity>& viableEntities,
    std::vector<entity_data>& entities, const int MAP_SIZE,
    std::shared_ptr<cg3d::Material> &basicMaterial,
    std::shared_ptr<cg3d::Material> &frameColor,
    std::shared_ptr<cg3d::Material> &collisionColor,
    std::shared_ptr<cg3d::Movable> &root, int currentLevel)
{
    int enemies, items = 10, bonuses = 2;
    switch (currentLevel)
    {
    case 1:
        enemies = 3;
        break;
    case 2:
        enemies = 5;
        break;
    case 3:
        enemies = 7;
        break;
    }
    bool spawnedEnemies = false, spawnedItems = false, spawnedBonuses = false;
    while (!(spawnedEnemies && spawnedItems && spawnedBonuses))
    {
        int index = getRandomNumberInRange(0, viableEntities.size());
        if (viableEntities[index].type == EntityType::ITEM && !spawnedItems)
        {
            spawnEntities(index, items, viableEntities, entities, MAP_SIZE, basicMaterial,
                frameColor, collisionColor, root, currentLevel);
            spawnedItems = true;
            continue;
        }
        if (viableEntities[index].type == EntityType::ENEMY && !spawnedEnemies)
        {
            spawnEntities(index, items, viableEntities, entities, MAP_SIZE, basicMaterial,
                frameColor, collisionColor, root, currentLevel);
            spawnedEnemies = true;
            continue;
        }
        if (viableEntities[index].type == EntityType::BONUS && !spawnedBonuses)
        {
            spawnEntities(index, items, viableEntities, entities, MAP_SIZE, basicMaterial,
                frameColor, collisionColor, root, currentLevel);
            spawnedBonuses = true;
            continue;
        }
    }
}

static void shouldLevelUp(int& currentLevel, int currentScore)
{
    bool leveledUp = false;
    switch (currentLevel)
    {
    case 1:
        if (currentScore >= 3000)
        {
            leveledUp = true;
            currentLevel++;
        }
        break;
    case 2:
        if (currentScore >= 6000)
        {
            leveledUp = true;
            currentLevel++;
        }
        break;
    case 3:
        if (currentScore >= 9000)
        {
            leveledUp = true;
            currentLevel++;
        }
        break;
    default:
        break;
    }
}

static void deleteEntityIfTimedOut(entity_data entity)
{
    time_t now = time(nullptr);
    if (now - entity.ent.lifeTime < entity.creationTime)
        return;
    //insert Lior's method
}