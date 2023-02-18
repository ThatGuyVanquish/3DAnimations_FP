#pragma once

#include "Visitor.h"
#include "Camera.h"
#include <utility>
#include <memory>
#include "Scene.h"
#include "Movable.h"


namespace cg3d
{

    class AnimationVisitor : public Visitor
    {
    public:
//        explicit AnimationVisitor(Scene *scene);

        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;
        void rotateXY(Model* model);
        void snakyLocomotion();
        void translateLocomotionCameras();
        void rotateBackCameras();

//        void Visit(Scene* _scene) override;


    private:
        Scene* scene;
        int prevRotatedCylIndex = 0;
        Eigen::Quaternionf prev_quat_x, prev_quat_y, prev_quat_z;

        // snaky locomotion
        float delay = 3.5;
        std::vector<float> directions = {0.025, 0.04, 0.055, 0.07, 0.085, 0.1, 0.1, 0.1, 0.1, 0.1,/* | */ 0.1, 0.1, 0.1, 0.1, 0.1, 0.085, 0.07, 0.055, 0.04, 0.025};
        int round = 9;
        int direction = -1;
    };

} // namespace cg3d
