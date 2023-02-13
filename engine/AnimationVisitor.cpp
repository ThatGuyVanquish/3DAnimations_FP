#include "AnimationVisitor.h"



namespace cg3d
{

    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {

        Visitor::Run(scene = _scene, camera);

    }

    void AnimationVisitor::Visit(Model* model)
    {
        Eigen::Matrix3f system = model->GetRotation().transpose();
        Eigen::Vector3f rotation_z, rotation_x, rotation_y, rotation_vec;
        rotation_z = Eigen::Vector3f(0, 0, 1);
        rotation_x = Eigen::Vector3f(1, 0, 0);
        rotation_y = Eigen::Vector3f(0, 1, 0);

        if (scene->gameplay.animate)
        {
            std::string modelsName = model->name;
            if (modelsName.starts_with("Cyl"))
            {
                int cylIndex = std::stoi(modelsName.substr(4));
                if (cylIndex == 0) // need to check what's the head of the snake
                {
                    model->TranslateInSystem(system, scene->gameplay.velocityVec);
                }
                else if (cylIndex == 1 && prevRotatedCylIndex == 0) {
//                    rotation_vec = model->Tout.rotation() * rotation_z;
//                    prev_quat_z = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_z);
//                    prev_quat_z = prev_quat_z.slerp(slerpFactor, Eigen::Quaternionf::Identity());
//                    model->Rotate(prev_quat_z); //might need rotate in system
                    rotation_vec = model->Tout.rotation() * rotation_x;
                    prev_quat_x = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_x);
                    prev_quat_x = prev_quat_x.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_x); //might need rotate in system
                    rotation_vec = model->Tout.rotation() * rotation_y;
                    prev_quat_y = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_y);
                    prev_quat_y = prev_quat_y.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_y); //might need rotate in system
                    prevRotatedCylIndex++;
                } else if (cylIndex == prevRotatedCylIndex + 1) {
                    model->Rotate(prev_quat_x.conjugate());
                    model->Rotate(prev_quat_y.conjugate());
//                    model->Rotate(prev_quat_z.conjugate());
                    rotation_vec = model->Tout.rotation() * rotation_x;
                    prev_quat_x = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_x);
                    prev_quat_x = prev_quat_x.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_x); //might need rotate in system
                    rotation_vec = model->Tout.rotation() * rotation_y;
                    prev_quat_y = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_y);
                    prev_quat_y = prev_quat_y.slerp(slerpFactor, Eigen::Quaternionf::Identity());
                    model->Rotate(prev_quat_y); //might need rotate in system
//                    rotation_vec = model->Tout.rotation() * rotation_z;
//                    prev_quat_z = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_z);
//                    prev_quat_z = prev_quat_z.slerp(slerpFactor, Eigen::Quaternionf::Identity());
//                    model->Rotate(prev_quat_z); //might need rotate in system
                    prevRotatedCylIndex++;
                }
                if (prevRotatedCylIndex >= scene->gameplay.numOfCyls-1)
                {
                    prevRotatedCylIndex = 0;
                    scene->gameplay.snakeSkinning.moveModel(scene->gameplay.cyls, scene->gameplay.snake);
                }
            }
        }
        Visitor::Visit(model);
    }


} // namespace cg3d
