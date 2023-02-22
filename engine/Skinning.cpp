//
// Created by Lior Levi on 13/02/2023.
//

#include "Skinning.h"


void Skinning::InitSkinning(model_data &snake, std::vector<model_data> &cyls)
{
    Eigen::MatrixXd initial_V = snake.model->GetMesh(0)->data[0].vertices;
    V = Eigen::MatrixXd::Zero(initial_V.rows(), initial_V.cols());
    numOfCyls = cyls.size();

    // set snake mesh in rest position
    Eigen::Matrix4d transform{Eigen::Matrix4d::Identity()};
    Eigen::Affine3d Tout{Eigen::Affine3d::Identity()}, Tin{Eigen::Affine3d::Identity()};
    Eigen::Vector3d translate, scale;
    translate = {0, 0, (1.6 * (numOfCyls / 2))};
//    scale = {1.0, 1.0, 16.0};
    Tout.pretranslate(translate);
//    Tin.scale(scale);
    transform = Tout.matrix() * Tin.matrix();

    for (int i = 0; i < initial_V.rows(); i++)
    {
        Eigen::Vector4d vertex = {initial_V(i, 0), initial_V(i, 1), initial_V(i, 2) ,1};
        vertex = transform * vertex;
        V(i, 0) = vertex[0];
        V(i, 1) = vertex[1];
        V(i, 2) = vertex[2];
    }

    // create new mesh based on transformed vertices
    std::shared_ptr<cg3d::Mesh> deformedMesh = std::make_shared<cg3d::Mesh>(snake.model->name,
                                                                            V,
                                                                            snake.model->GetMesh(0)->data[0].faces,
                                                                            snake.model->GetMesh(0)->data[0].vertexNormals,
                                                                            snake.model->GetMesh(0)->data[0].textureCoords
    );
    // change snake mesh to the transformed one
    snake.model->SetMeshList({deformedMesh});

    // init vT, vQ, Positions
    InitTransformations(cyls);

    // calculate weights
    calcWeight();

}

void Skinning::InitTransformations(std::vector<model_data> &cyls)
{
    // init vT and vQ
    vQ.resize(numOfCyls + 1 , Eigen::Quaterniond::Identity());
    for (int i = 0; i < vQ.size(); i++) {
        Eigen::Quaterniond quad = vQ[i];
    }
    vT.resize(numOfCyls + 1);
    Positions.resize(numOfCyls + 1);
    Eigen::Vector3f cylPos;
    for (int i = 0; i < numOfCyls; i++)
    {
        cylPos = getPosition(cyls[i], -0.8f);
        Positions[i] = cylPos.cast<double>();
    }
    cylPos = getPosition(cyls[numOfCyls - 1], 0.8f);
    Positions[numOfCyls] = cylPos.cast<double>();
}

void Skinning::calcWeight()
{
    int n = V.rows();
    W = Eigen::MatrixXd::Zero(n, numOfCyls + 1);

    snakeLength = V.colwise().maxCoeff()[2] - V.colwise().minCoeff()[2];
    jointLength = snakeLength / numOfCyls;

    double min_z = V.colwise().minCoeff()[2];

    for (int i = 0; i < n; i++) {
        double curr_z = V.row(i)[2];
//        for (int j = 0; j < numOfCyls + 1; j++) {
//            if (curr_z >= min_z + jointLength * j && curr_z <= min_z + jointLength * (j + 1)) {
//                double dist = abs(curr_z - (min_z + jointLength * j));
//                W.row(i)[j] = (jointLength - dist) / jointLength;
//                W.row(i)[j + 1] = 1 - W.row(i)[j];
//                break;
//            }
//        }
        double totalWeight = 0.0;
        double min_value_1 = std::numeric_limits<double>::infinity();
        double min_value_2 = std::numeric_limits<double>::infinity();
        double min_value_3 = std::numeric_limits<double>::infinity();
        int index_1 = 0;
        int index_2 = 0;
        int index_3 = 0;
        for (int j = 0; j < numOfCyls + 1; j++) {
            double dist = abs(curr_z - (min_z + jointLength * j));
            if (dist < min_value_1) {
                min_value_3 = min_value_2;
                index_3 = index_2;
                min_value_2 = min_value_1;
                index_2 = index_1;
                min_value_1 = dist;
                index_1 = j;
            } else if (dist < min_value_2) {
                min_value_3 = min_value_2;
                index_3 = index_2;
                min_value_2 = dist;
                index_2 = j;
            } else if (dist < min_value_3) {
                min_value_3 = dist;
                index_3 = j;
            }
        }
        double variance = 0.1;
        totalWeight = exp(-min_value_1 * min_value_1 / (2.0 * variance)) + exp(-min_value_2 * min_value_2 / (2.0 * variance)) + exp(-min_value_3 * min_value_3 / (2.0 * variance));
        W.row(i)[index_1] = exp(-min_value_1 * min_value_1 / (2.0 * variance)) / totalWeight;
        W.row(i)[index_2] = exp(-min_value_2 * min_value_2 / (2.0 * variance)) / totalWeight;
        W.row(i)[index_3] = exp(-min_value_3 * min_value_3 / (2.0 * variance)) / totalWeight;

//            double sum = min_value_1 + min_value_2 + min_value_3;
//            W.row(i)[index_1] = (min_value_3 / sum);
//            W.row(i)[index_2] = (min_value_2 / sum);
//            W.row(i)[index_3] = (min_value_1 / sum);

//            W.row(i)[j] = snakeLength - dist;
//            double variance = 1.0;
//            W.row(i)[j] = exp(-dist * dist / (2.0 * variance));
//            totalWeight += W.row(i)[j];

//        for (int j = 0; j < numOfCyls + 1; j++) {
//            W.row(i)[j] /= totalWeight;
//        }
    }
    std::cout << "W: " << W << std::endl;
}

void Skinning::moveModel(std::vector<model_data> &cyls, model_data &snake)
{
    // update vT with cylinders' joints positions
    Eigen::Vector3f jointPos;
    for (int i = 0; i < numOfCyls; i++)
    {
        jointPos = getPosition(cyls[i], -0.8f);
        vT[i] = jointPos.cast<double>() - Positions[i];
    }
    jointPos = getPosition(cyls[numOfCyls - 1], 0.8f);
    vT[numOfCyls] = jointPos.cast<double>() - Positions[numOfCyls];

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


