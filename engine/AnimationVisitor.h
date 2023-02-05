#pragma once

#include "Visitor.h"
#include "Camera.h"

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
        Eigen::Quaternionf prevRotationQuaternion;
        float slerpFactor = 0.99f;
    };

} // namespace cg3d
