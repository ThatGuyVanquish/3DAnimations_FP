#pragma once

#include "Visitor.h"
#include "Camera.h"
#include <utility>
#include <memory>
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"


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
        Eigen::Quaternionf prev_quat_x, prev_quat_y, prev_quat_z;
        float slerpFactor = 0.9f;

    };

} // namespace cg3d
