#pragma once

#include "Visitor.h"
#include "Camera.h"
#include "Skinning.cpp"
#include <utility>
#include <memory>


namespace cg3d
{

    class AnimationVisitor : public Visitor
    {
    public:
//        explicit AnimationVisitor(Scene *scene);

        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;
//        void Visit(Scene* _scene) override;


    private:
        Scene* scene;
        int prevRotatedCylIndex = 0;
        Eigen::Quaternionf prev_quat_z;
        float slerpFactor = 0.9f;
        Eigen::Vector3f velocityVec = {0, 0, -0.05f};
        Eigen::MatrixXd W;
        bool calc_w = true;
        bool doSkinning = true;
    };

} // namespace cg3d
