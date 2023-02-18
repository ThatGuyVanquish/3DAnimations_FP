#pragma once

#include <igl/arap.h>
#include <igl/boundary_loop.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>
#include <igl/lscm.h>
#include <igl/parallel_for.h>


static void setAllUVs(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F_true, std::vector<Eigen::MatrixXd> &uv_vec)
{
    int size = F_true.rows();
    uv_vec.resize(size);
    igl::parallel_for(size,[&](const int j)
    {
        Eigen::MatrixXi F;
        F.resize(F_true.rows()-1, 3);
        bool visited = false;
        for (int i = 0; i < F_true.rows(); i++)
        {
            if (i != j) {
                if (visited) F.row(i-1) = F_true.row(i);
                else F.row(i) = F_true.row(i);
            } else visited = true;
        }
        Eigen::MatrixXd V_uv;
        // 503
        Eigen::MatrixXd initial_guess;
        // Compute the initial solution for ARAP (harmonic parametrization)
        Eigen::VectorXi bnd;
        igl::boundary_loop(F,bnd);
        Eigen::MatrixXd bnd_uv;
        igl::map_vertices_to_circle(V,bnd,bnd_uv);

        igl::harmonic(V,F,bnd,bnd_uv,1,initial_guess);

        // Add dynamic regularization to avoid to specify boundary conditions
        igl::ARAPData arap_data;
        arap_data.with_dynamics = true;
        Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
        Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);

        // Initialize ARAP
        arap_data.max_iter = 100;
        // 2 means that we're going to *solve* in 2d
        arap_precomputation(V,F,2,b,arap_data);


        // Solve arap using the harmonic map as initial guess
        V_uv = initial_guess;

        arap_solve(bc,arap_data,V_uv);


        // Scale UV to make the texture more clear
        V_uv *= 20;
        uv_vec[j] = V_uv;
    },5);
//    for (int j = 0; j < size; j++)
//    {
//        Eigen::MatrixXi F;
//        F.resize(F_true.rows()-1, 3);
//        bool visited = false;
//        for (int i = 0; i < F_true.rows(); i++)
//        {
//            if (i != j) {
//                if (visited) F.row(i-1) = F_true.row(i);
//                else F.row(i) = F_true.row(i);
//            } else visited = true;
//        }
//        Eigen::MatrixXd V_uv;
//        // 503
//        Eigen::MatrixXd initial_guess;
//        // Compute the initial solution for ARAP (harmonic parametrization)
//        Eigen::VectorXi bnd;
//        igl::boundary_loop(F,bnd);
//        Eigen::MatrixXd bnd_uv;
//        igl::map_vertices_to_circle(V,bnd,bnd_uv);
//
//        igl::harmonic(V,F,bnd,bnd_uv,1,initial_guess);
//
//        // Add dynamic regularization to avoid to specify boundary conditions
//        igl::ARAPData arap_data;
//        arap_data.with_dynamics = true;
//        Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
//        Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);
//
//        // Initialize ARAP
//        arap_data.max_iter = 100;
//        // 2 means that we're going to *solve* in 2d
//        arap_precomputation(V,F,2,b,arap_data);
//
//
//        // Solve arap using the harmonic map as initial guess
//        V_uv = initial_guess;
//
//        arap_solve(bc,arap_data,V_uv);
//
//
//        // Scale UV to make the texture more clear
//        V_uv *= 20;
//        uv_vec[j] = V_uv;
//    }
}

static void setUV(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F_true, Eigen::MatrixXd &V_uv)
{

    int rows = F_true.rows()-1;
//    Eigen::MatrixXi F = F_true.block(0,0,rows,3);
    Eigen::MatrixXi F = F_true;

//    int counter = 0;
//    for (int i = 0; i < F.rows(); i++)
//    {
//        if ((abs(V.row(F(i, 0))[2]) < 0.09 || abs(V.row(F(i, 1))[2]) < 0.09 || abs(V.row(F(i, 2))[2]) < 0.09) &&
//                (abs(V.row(F(i, 0))[0]) < 0.09 || abs(V.row(F(i, 1))[0]) < 0.09 || abs(V.row(F(i, 2))[0]) < 0.09) &&
//                (V.row(F(i, 0))[1] < -0.3 || V.row(F(i, 1))[1] < -0.3 || V.row(F(i, 2))[1] < -0.3)){
//            std::cout << "face index = " << i << std::endl;
//            std::cout << V.row(F(i, 0)).transpose() << "\n" << std::endl;
//            std::cout << V.row(F(i, 1)).transpose() << "\n" << std::endl;
//            std::cout << V.row(F(i, 2)).transpose() << "\n" << std::endl;
//            std::cout << "\n" << std::endl;
//            counter++;
//        }
//    }
//    std::cout << counter << std::endl;
//    std::cout << V.colwise().maxCoeff() << std::endl;
//    std::cout << V.colwise().minCoeff() << std::endl;

    F.resize(F_true.rows()-1, 3);
    bool visited = false;
    for (int i = 0; i < F_true.rows(); i++)
    {
       if (i != 711) {
           if (visited) F.row(i-1) = F_true.row(i);
           else F.row(i) = F_true.row(i);
       } else visited = true;
    }

//    // 501
//    // Find the open boundary
//    Eigen::VectorXi bnd;
//    igl::boundary_loop(F,bnd);
//
//    // Map the boundary to a circle, preserving edge proportions
//    Eigen::MatrixXd bnd_uv;
//    igl::map_vertices_to_circle(V,bnd,bnd_uv);
//
//    // Harmonic parametrization for the internal vertices
//    igl::harmonic(V,F,bnd,bnd_uv,1,V_uv);
//
//    // Scale UV to make the texture more clear
//    V_uv *= 5;


//    // 502
//    // Fix two points on the boundary
//    Eigen::VectorXi bnd,b(2,1);
//    igl::boundary_loop(F,bnd);
//    b(0) = bnd(0);
//    b(1) = bnd(bnd.size()/2);
//    Eigen::MatrixXd bc(2,2);
//    bc<<0,0,1,0;
//
//    // LSCM parametrization
//    igl::lscm(V,F,b,bc,V_uv);
//
//    // Scale the uv
//    V_uv *= 5;


    // 503
    Eigen::MatrixXd initial_guess;
    // Compute the initial solution for ARAP (harmonic parametrization)
    Eigen::VectorXi bnd;
    igl::boundary_loop(F,bnd);
    Eigen::MatrixXd bnd_uv;
    igl::map_vertices_to_circle(V,bnd,bnd_uv);

    igl::harmonic(V,F,bnd,bnd_uv,1,initial_guess);

    // Add dynamic regularization to avoid to specify boundary conditions
    igl::ARAPData arap_data;
    arap_data.with_dynamics = true;
    Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
    Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);

    // Initialize ARAP
    arap_data.max_iter = 100;
    // 2 means that we're going to *solve* in 2d
    arap_precomputation(V,F,2,b,arap_data);


    // Solve arap using the harmonic map as initial guess
    V_uv = initial_guess;

    arap_solve(bc,arap_data,V_uv);


    // Scale UV to make the texture more clear
//    V_uv *= 20;
//    V_uv *= 10;

}