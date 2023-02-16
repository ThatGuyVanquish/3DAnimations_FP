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
                if (cylIndex == 0) {
                    if ((time(nullptr) - scene->gameplay.timeFromLastWASDQE) > 2.5) snakyLocomotion();
                    model->TranslateInSystem(system, scene->gameplay.velocityVec);
//                    translateLocomotionCameras();
                } else if (cylIndex == 1 && prevRotatedCylIndex == 0) {
                    rotateXY(model);
                } else if (cylIndex == prevRotatedCylIndex + 1) {
                    model->Rotate(prev_quat_x.conjugate());
                    model->Rotate(prev_quat_y.conjugate());
                    rotateXY(model);
                }

                if (prevRotatedCylIndex >= scene->gameplay.numOfCyls-1)
                {
                    prevRotatedCylIndex = 0;
                    if (scene->gameplay.useSnake) scene->gameplay.snakeSkinning.moveModel(scene->gameplay.cyls, scene->gameplay.snake);
                }
            }
        }
        Visitor::Visit(model);
    }

    void AnimationVisitor::rotateXY(Model* model)
    {
        Eigen::Vector3f rotation_z, rotation_x, rotation_y, rotation_vec;
        rotation_z = Eigen::Vector3f(0, 0, 1);
        rotation_x = Eigen::Vector3f(1, 0, 0);
        rotation_y = Eigen::Vector3f(0, 1, 0);

        rotation_vec = model->Tout.rotation() * rotation_x;
        prev_quat_x = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_x);
        prev_quat_x = prev_quat_x.slerp(scene->gameplay.slerpFactor, Eigen::Quaternionf::Identity());
        model->Rotate(prev_quat_x);
        rotation_vec = model->Tout.rotation() * rotation_y;
        prev_quat_y = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_y);
        prev_quat_y = prev_quat_y.slerp(scene->gameplay.slerpFactor, Eigen::Quaternionf::Identity());
        model->Rotate(prev_quat_y);
        prevRotatedCylIndex++;
    }

    void AnimationVisitor::snakyLocomotion()
    {
        if (scene->gameplay.slerpFactor != 0.8) {
            scene->gameplay.prev_slerp = scene->gameplay.slerpFactor;
            scene->SetCamera(scene->cameraIdx);
        }
        scene->gameplay.slerpFactor = 0.8;
        scene->gameplay.cyls[0].model->Rotate(direction*directions[round], Movable::Axis::Y);
        scene->gameplay.cyls[1].model->Rotate(-1*direction*directions[round], Movable::Axis::Y);
        scene->cameras[1]->Rotate(-1*direction*directions[round], Movable::Axis::Y);
        scene->cameras[2]->Rotate(-1*direction*directions[round], Movable::Axis::Y);
        round += direction;
        if (round < 0 || round >= directions.size()) {
            direction *= -1;
            round += direction;
        }
    }

    void AnimationVisitor::translateLocomotionCameras()
    {
        Eigen::Vector3f translate = getPosition(scene->gameplay.cyls[0], -0.8) - scene->gameplay.currPos;
        scene->gameplay.currPos = getPosition(scene->gameplay.cyls[0], -0.8);
        scene->locomotionCameras[0]->Translate(translate);
        scene->locomotionCameras[1]->Translate(translate);
        scene->locomotionCameras[2]->Translate(translate);
    }



} // namespace cg3d
