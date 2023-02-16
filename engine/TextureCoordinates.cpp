#include "TextureCoordinates.h"

//static void setUV(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixXd &V_uv)
//{
//    Eigen::MatrixXd initial_guess;
//    // Compute the initial solution for ARAP (harmonic parametrization)
//    Eigen::VectorXi bnd;
//    igl::boundary_loop(F,bnd);
//    Eigen::MatrixXd bnd_uv;
//    igl::map_vertices_to_circle(V,bnd,bnd_uv);
//
//    igl::harmonic(V,F,bnd,bnd_uv,1,initial_guess);
//
//    // Add dynamic regularization to avoid to specify boundary conditions
//    igl::ARAPData arap_data;
//    arap_data.with_dynamics = true;
//    Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
//    Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);
//
//    // Initialize ARAP
//    arap_data.max_iter = 100;
//    // 2 means that we're going to *solve* in 2d
//    arap_precomputation(V,F,2,b,arap_data);
//
//
//    // Solve arap using the harmonic map as initial guess
//    V_uv = initial_guess;
//
//    arap_solve(bc,arap_data,V_uv);
//
//
//    // Scale UV to make the texture more clear
//    V_uv *= 20;
//}