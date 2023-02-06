//
// Created by Lior Levi on 06/02/2023.
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
#include "Skinning.h"
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

class Skinning {

public:
    void applySkinning(Eigen::MatrixXd &W, std::vector<model_data> &cyls, model_data &snake);
    void calculateWeights(Eigen::MatrixXd &W, std::vector<model_data> &cyls, model_data &snake);

};
