//
// Created by Lior Levi on 06/02/2023.
//

#pragma once

#include "IglMeshLoader.h"
#include <AABB.h>
#include "Model.h"

typedef struct {
    std::shared_ptr<cg3d::Model> model;
    float scaleFactor;
    igl::AABB<Eigen::MatrixXd, 3> aabb;
    std::shared_ptr<cg3d::Model> collisionFrame;
    std::shared_ptr<cg3d::Model> collisionBox;
} model_data;
