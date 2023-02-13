//
// Created by Lior Levi on 13/02/2023.
//
#pragma once
#include <utility>
#include <Eigen/Core>
#include <Eigen/LU>
#include <igl/opengl/glfw/Viewer.h>
#include "../engine/Mesh.h"
#include <igl/AABB.h>
#include <GLFW/glfw3.h>
#include <igl/per_vertex_normals.h>
#include <Movable.h>
#include <igl/directed_edge_orientations.h>
#include <igl/directed_edge_parents.h>
#include <igl/forward_kinematics.h>
#include <igl/PI.h>
#include <igl/lbs_matrix.h>
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/readDMAT.h>
#include <igl/readOBJ.h>
#include <igl/readTGF.h>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <algorithm>
#include <iostream>
#include "common.h"

typedef
std::vector<Eigen::Quaterniond,Eigen::aligned_allocator<Eigen::Quaterniond> >
        RotationList;

static Eigen::Vector3f getPosition(model_data & cyl, float offset)
{
    Eigen::Vector3f center = cyl.model->GetAggregatedTransform().col(3).head(3);
    Eigen::Vector3f translation{ 0, 0, offset };
    Eigen::Vector3f tipPosition = center + cyl.model->GetRotation() * translation;
    return tipPosition;
}

static void print_min_max(const Eigen::MatrixXd& matrix);

class Skinning {

public:

    void InitSkinning(model_data &snake, std::vector<model_data> &cyls);
    void InitTransformations(std::vector<model_data> &cyls);
    void calcWeight();
    void moveModel(std::vector<model_data> &cyls, model_data &snake);
    void moveModel2(Eigen::Vector3d t, std::vector<model_data> &cyls, model_data &snake);

    bool initVT = true;
    bool initSnake = true;
    int numOfCyls;
    float snakeLength, jointLength;
    Eigen::MatrixXd W, V, U;
    std::vector<Eigen::Vector3d> vT, Positions;

    RotationList vQ;
};


