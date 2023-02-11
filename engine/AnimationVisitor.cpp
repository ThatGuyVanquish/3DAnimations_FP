#include "AnimationVisitor.h"
#include "Visitor.h"
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"


namespace cg3d
{

    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {

        Visitor::Run(scene = _scene, camera);

    }

    void AnimationVisitor::Visit(Model* model)
    {
        Eigen::Matrix3f system = model->GetRotation().transpose();
        Eigen::Vector3f rotation_z, rotation_vec;
        rotation_z = Eigen::Vector3f(0, 0, 1);

        if (scene->gameplay.animate)
        {
            std::string modelsName = model->name;
            if (modelsName.starts_with("SNAKE"))
            {
//                if (calc_w)
//                {
//                    calculateWeights(W, scene->cyls, scene->snake);
//                    calc_w = false;
//                }
//                applySkinning(W, scene->cyls, scene->snake);
////                doSkinning = false;
            }
            else if (modelsName.starts_with("Cyl"))
            {
                int cylIndex = std::stoi(modelsName.substr(4));
                if (cylIndex == 0) // need to check what's the head of the snake
                {
                    model->TranslateInSystem(system, velocityVec);
                }
                else if (cylIndex == 1 && prevRotatedCylIndex == 0) {
                    rotation_vec = model->Tout.rotation() * rotation_z;
                    prev_quat_z = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_z);
                    prev_quat_z = prev_quat_z.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_z); //might need rotate in system
                    prevRotatedCylIndex++;
                } else if (cylIndex == prevRotatedCylIndex + 1) {
                    model->Rotate(prev_quat_z.conjugate());
                    rotation_vec = model->Tout.rotation() * rotation_z;
                    prev_quat_z = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_z);
                    prev_quat_z = prev_quat_z.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_z); //might need rotate in system
                    prevRotatedCylIndex++;
                }
                if (prevRotatedCylIndex >= scene->gameplay.numOfCyls-1)
                {
                    prevRotatedCylIndex = 0;
//                    doSkinning = true;
                }
            }
        }
        Visitor::Visit(model);
    }


} // namespace cg3d
