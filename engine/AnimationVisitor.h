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
        int rotatedCylIndex = 0;
        Eigen::Quaternionf prevRotationQuaternion;
    };

} // namespace cg3d
