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




static void applySkinning(Eigen::MatrixXd &W, std::vector<model_data> &cyls, model_data &snake)
{
    // W - weights matrix
    // BE - Edges between joints
    // C - joints positions
    // P - parents
    // M - weights per vertex per joint matrix
    // U - new vertices position after skinning
//    Eigen::MatrixXd V,W,C,U,M;
//    Eigen::MatrixXi BE;
//    Eigen::VectorXi P;
//    std::vector<RotationList > poses; // rotations of joints for animation

    RotationList vQ;
    std::vector<Eigen::Vector3d> vT;
    Eigen::MatrixXd U, V;
    Eigen::MatrixXi BE;
    Eigen::VectorXi P;
    Eigen::MatrixXd C;

    int joints = cyls.size();
    float h = (1.6f/joints);
    float a = -0.8f;
    C.resize(joints+1, 3);
    BE.resize(joints, 2);
    P.resize(joints, 1);
    RotationList anim_pose(joints);
    for (int i = 0; i < joints; i++)
    {
        C(i, 0) = 0;
        C(i, 1) = 0;
        C(i, 2) = a+(i*h);
        BE(i, 0) = i;
        BE(i, 1) = i+1;
        if (i == 0) P(i) = -1;
        else P(i) = i-1;
        anim_pose[i] = Eigen::Quaterniond(cyls[i].model->GetTout().rotation().cast<double>());
    }
    C(joints, 0) = 0;
    C(joints, 1) = 0;
    C(joints, 2) = 0.8;

    igl::forward_kinematics(C,BE,P,anim_pose,vQ,vT);

//    vQ.resize(cyls.size());
//    vT.resize(cyls.size());
    for (int cyl = 0; cyl < cyls.size(); cyl++)
    {
//        vQ[cyl] = Eigen::Quaterniond(cyls[cyl].model->GetRotation().cast<double>());
////        vQ[cyl] = Eigen::Quaterniond(cyls)
        // no translation needed
        vT[cyl] = Eigen::Vector3d(0,0,0);
    }

    V = snake.model->GetMesh(0)->data[0].vertices;

    // activate Dual Quaternion Skinning
    igl::dqs(V,W,vQ,vT,U);

    // create new mesh based on deformed vertex
    std::shared_ptr<cg3d::Mesh> deformedMesh = std::make_shared<cg3d::Mesh>(snake.model->name,
                                                                   U,
                                                                   snake.model->GetMesh(0)->data[0].faces,
                                                                   snake.model->GetMesh(0)->data[0].vertexNormals,
                                                                   snake.model->GetMesh(0)->data[0].textureCoords
                                                                   );

    // change snake mesh to the deformed one
    snake.model->SetMeshList({deformedMesh});
}

static void calculateWeights(Eigen::MatrixXd &W, std::vector<model_data> &cyls, model_data &snake)
{
    Eigen::MatrixXd V = snake.model->GetMesh(0)->data[0].vertices;
    int joints = cyls.size();
    int vertices = V.rows();
    W.resize(vertices, joints);

    // calculate center of joints rest position
    std::vector<Eigen::Vector3d> jointsPos;
    float h = (1.6f/joints);
    float a = -0.8f + h/2;
    for ( int i = 0; i < joints; i++)
    {
        jointsPos.push_back({0, 0, a + (i * h)});
    }

    // calculate weights
    for (int i = 0; i < W.rows(); i++)
    {
        double totalWeight = 0.0;
        Eigen::Vector3d vertex_pos = V.row(i);
        Eigen::Vector3d joint_pos;
        double variance = 0.002;
        for (int j = 0; j < W.cols(); j++)
        {
            joint_pos = jointsPos[j];
            double distance = (vertex_pos - joint_pos).norm();
            if (abs(vertex_pos[2] - joint_pos[2]) <= h)
            {
                W(i,j) = exp((-distance * distance) / (2.0 * variance));
            } else {
                W(i,j) = 0.0;
            }
            totalWeight += W(i,j);
        }

        for (int j = 0; j < W.cols(); j++)
        {
            W(i, j) /= totalWeight;
        }
    }
//    std::cout << "W: " << W << std::endl;
    std::cout << "W.row(145): " << W.row(145) << std::endl;


}

//const Eigen::RowVector3d sea_green(70./255.,252./255.,167./255.);
//// W - weights matrix
//// BE - Edges between joints
//// C - joints positions
//// P - parents
//// M - weights per vertex per joint matrix
//// U - new vertices position after skinning
//Eigen::MatrixXd V,W,C,U,M;
//Eigen::MatrixXi F,BE;
//Eigen::VectorXi P;
//std::vector<RotationList > poses; // rotations of joints for animation
//double anim_t = 0.0;
//double anim_t_dir = 0.015;
//bool use_dqs = false;
//bool recompute = true;
//
//
//bool pre_draw(igl::opengl::glfw::Viewer & viewer)
//{
//    using namespace Eigen;
//    using namespace std;
//    if(recompute)
//    {
//        // Find pose interval
//        const int begin = (int)floor(anim_t)%poses.size();
//        const int end = (int)(floor(anim_t)+1)%poses.size();
//        const double t = anim_t - floor(anim_t);
//
//        // Interpolate pose and identity
//        RotationList anim_pose(poses[begin].size());
//        for(int e = 0;e<poses[begin].size();e++)
//        {
//            anim_pose[e] = poses[begin][e].slerp(t,poses[end][e]);
//        }
//        // Propagate relative rotations via FK to retrieve absolute transformations
//        // vQ - rotations of joints
//        // vT - translation of joints
//        RotationList vQ;
//        vector<Vector3d> vT;
//        igl::forward_kinematics(C,BE,P,anim_pose,vQ,vT);
//        const int dim = C.cols();
//        MatrixXd T(BE.rows()*(dim+1),dim);
//        for(int e = 0;e<BE.rows();e++)
//        {
//            Affine3d a = Affine3d::Identity();
//            a.translate(vT[e]);
//            a.rotate(vQ[e]);
//            T.block(e*(dim+1),0,dim+1,dim) =
//                    a.matrix().transpose().block(0,0,dim+1,dim);
//        }
//        // Compute deformation via LBS as matrix multiplication
//        if(use_dqs)
//        {
//            igl::dqs(V,W,vQ,vT,U);
//        }else
//        {
//            U = M*T;
//        }
//
//        // Also deform skeleton edges
//        MatrixXd CT;
//        MatrixXi BET;
//        //move joints according to T, returns new position in CT and BET
//        std::cout << "before" << std::endl;
//        std::cout << BE << std::endl;
//        igl::deform_skeleton(C,BE,T,CT,BET);
//        std::cout << "after" << std::endl;
//        std::cout << BE << std::endl;
//        std::cout << BET << std::endl;
//        viewer.data().set_vertices(U);
//        viewer.data().set_edges(CT,BET,sea_green);
//        viewer.data().compute_normals();
//        if(viewer.core().is_animating)
//        {
//            anim_t += anim_t_dir;
//        }
//        else
//        {
//            recompute=false;
//        }
//    }
//    return false;
//}
//
//bool key_down(igl::opengl::glfw::Viewer &viewer, unsigned char key, int mods)
//{
//    recompute = true;
//    switch(key)
//    {
//        case 'D':
//        case 'd':
//            use_dqs = !use_dqs;
//            return true;
//        case ' ':
//            viewer.core().is_animating = !viewer.core().is_animating;
//            return true;
//    }
//    return false;
//}
//
//int mainloop()
//{
//    using namespace Eigen;
//    using namespace std;
////    igl::readOBJ(TUTORIAL_SHARED_PATH "/arm.obj",V,F);
//    U=V;
////    igl::readTGF(TUTORIAL_SHARED_PATH "/arm.tgf",C,BE);
//    // retrieve parents for forward kinematics
//    igl::directed_edge_parents(BE,P);
//    RotationList rest_pose;
//    igl::directed_edge_orientations(C,BE,rest_pose);
//    poses.resize(4,RotationList(4,Quaterniond::Identity()));
//    // poses[1] // twist
//    const Quaterniond twist(AngleAxisd(igl::PI,Vector3d(1,0,0)));
//    poses[1][2] = rest_pose[2]*twist*rest_pose[2].conjugate();
//    const Quaterniond bend(AngleAxisd(-igl::PI*0.7,Vector3d(0,0,1)));
//    poses[3][2] = rest_pose[2]*bend*rest_pose[2].conjugate();
//
////    igl::readDMAT(TUTORIAL_SHARED_PATH "/arm-weights.dmat",W);
//    igl::lbs_matrix(V,W,M);
//
//    // Plot the mesh with pseudocolors
//    igl::opengl::glfw::Viewer viewer;
//    viewer.data().set_mesh(U, F);
//    viewer.data().set_edges(C,BE,sea_green);
//    viewer.data().show_lines = false;
//    viewer.data().show_overlay_depth = false;
//    viewer.data().line_width = 1;
//    viewer.core().trackball_angle.normalize();
//    viewer.callback_pre_draw = &pre_draw;
//    viewer.callback_key_down = &key_down;
//    viewer.core().is_animating = false;
//    viewer.core().camera_zoom = 2.5;
//    viewer.core().animation_max_fps = 30.;
//    cout<<"Press [d] to toggle between LBS and DQS"<<endl<<
//        "Press [space] to toggle animation"<<endl;
//    viewer.launch();
//}
//
