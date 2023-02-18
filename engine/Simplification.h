#pragma once

#include <utility>
#include "IglMeshLoader.h"
#include <igl/collapse_edge.h>
#include <igl/edge_flaps.h>
#include <igl/shortest_edge_and_midpoint.h>
#include <igl/parallel_for.h>
#include <igl/read_triangle_mesh.h>
#include "../engine/Mesh.h"


static std::shared_ptr<cg3d::Mesh> createDecimatedMesh(std::string filename)
{
    auto currentMesh{ cg3d::IglLoader::MeshFromFiles("currentMesh", filename) };
    Eigen::MatrixXd V, OV;
    Eigen::MatrixXi F, OF;
    OV = currentMesh->data[0].vertices;
    OF = currentMesh->data[0].faces;

    // Prepare array-based edge data structures and priority queue
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi E, EF, EI;
    igl::min_heap< std::tuple<double, int, int>> Q;
    Eigen::VectorXi EQ;
    // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd C;

    // Function to reset original mesh and data structures
    const auto& reset = [&]()
    {
        F = OF;
        V = OV;
        igl::edge_flaps(F, E, EMAP, EF, EI);
        C.resize(E.rows(), V.cols());
        Q = {};
        EQ = Eigen::VectorXi::Zero(E.rows());
        {
            Eigen::VectorXd costs(E.rows());
            igl::parallel_for(E.rows(), [&](const int e)
            {
                double cost = e;
                Eigen::RowVectorXd p(1, 3);
                igl::shortest_edge_and_midpoint(e, V, F, E, EMAP, EF, EI, cost, p);
                C.row(e) = p;
                costs(e) = cost;
            }, 10000);
            for (int e = 0; e < E.rows(); e++)
            {
                Q.emplace(costs(e), e, 0);
            }
        }
    };

    reset();
    int current_num_of_edges = E.rows();
    int collapsed_edges = 0;
    const int max_iter = 0.3 * current_num_of_edges;
    for (int j = 0; j < max_iter; j++)
    {
        if (!igl::collapse_edge(igl::shortest_edge_and_midpoint, V, F, E, EMAP, EF, EI, Q, EQ, C))
        {
            break;
        }
        // 3 because in every collapse there are 3 edges deleted from E
        collapsed_edges += 3;
    }
    if (collapsed_edges > 0) {
        current_num_of_edges -= collapsed_edges;
        currentMesh->data.push_back({ V, F, currentMesh->data[0].vertexNormals, currentMesh->data[0].textureCoords });
    }
    return currentMesh;
}
