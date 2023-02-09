//
// Created by Lior Levi on 06/02/2023.
//

#pragma once

#include "../tutorial/Assignment4/IglMeshLoader.h"
#include <AABB.h>
#include "Model.h"
#include <chrono>

typedef struct {
    std::shared_ptr<cg3d::Model> model;
    float scaleFactor;
    igl::AABB<Eigen::MatrixXd, 3> aabb;
    std::shared_ptr<cg3d::Model> collisionFrame;
    std::shared_ptr<cg3d::Model> collisionBox;
} model_data;

enum class EntityType { ITEM, BONUS, ENEMY };

typedef struct {
    const std::string name;
    const std::string pathToMesh;
    const float scale;
    const EntityType type;
    const int points;
    const int lifeTime;
} Entity;

typedef struct {
    model_data model;
    const std::chrono::time_point<std::chrono::steady_clock> creationTime;
    const Entity ent;
} entity_data;

static std::string entityTypeToString(EntityType type)
{
    switch (type)
    {
    case EntityType::ITEM: return "Item_";
    case EntityType::BONUS: return "Bonus_";
    case EntityType::ENEMY: return "Enemy_";
    }
    return "";
}