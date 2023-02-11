//
// Created by Lior Levi on 06/02/2023.
//

#pragma once

#include "../tutorial/Assignment4/IglMeshLoader.h"
#include <AABB.h>
#include "Model.h"
#include <chrono>
#include <random>

#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)

typedef struct {
    std::shared_ptr<cg3d::Model> model;
    float scaleFactor;
    igl::AABB<Eigen::MatrixXd, 3> aabb;
    std::shared_ptr<cg3d::Model> collisionFrame;
    std::shared_ptr<cg3d::Model> collisionBox;
} model_data;

enum class EntityType { ITEM, BONUS, ENEMY };

typedef struct {
    std::string name;
    std::string pathToMesh;
    float scale;
    EntityType type;
    int points;
    int lifeTime;
} Entity;

typedef struct {
    model_data modelData;
    time_t creationTime;
    Entity ent;
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

static int getRandomNumberInRange(int low, int high)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(low, high - 1); // define the range
    return distr(gen);
}