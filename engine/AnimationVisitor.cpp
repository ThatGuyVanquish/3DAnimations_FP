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

        if (scene->gameplay.animate)
        {
            std::string modelsName = model->name;
            if (modelsName.starts_with("Cyl"))
            {
                int cylIndex = std::stoi(modelsName.substr(4));
                if (cylIndex == 0) {
                    if ((time(nullptr) - scene->gameplay.imGuiOverlay.timeFromLastWASDQE) > delay) snakyLocomotion();
                    model->TranslateInSystem(system, scene->gameplay.velocityVec);
                    translateLocomotionCameras();
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
            else if (modelsName.starts_with("Entity"))
            {
                if (!CheckInRange(model->initialTranslation, model->GetTranslation(), Eigen::Vector3f{0.0f, 1.0f, 0.0f}))
                    model->speed *= -1;
                model->Translate(model->speed);
            }
            Visitor::Visit(model);
        }
//        Visitor::Visit(model);
    }

    bool AnimationVisitor::CheckInRange(Eigen::Vector3f init, Eigen::Vector3f current, Eigen::Vector3f limit)
    {
        Eigen::Vector3f upper = init + limit;
        Eigen::Vector3f lower = init - limit;
        bool lessThanUpper = (upper - current).x() >= 0 && (upper - current).y() >= 0 && (upper - current).z() >= 0;
        bool moreThanLower = (current - lower).x() >= 0 && (current - lower).y() >= 0 && (current - lower).z() >= 0;
        return lessThanUpper && moreThanLower;
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
//        rotateBackCameras();
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
        for (int i = 0; i < scene->locomotionCameras.size(); i++)
        {

            Eigen::Vector3f D = getPosition(scene->gameplay.cyls[0], -0.8);
            Eigen::Vector3f R = getPosition(scene->gameplay.cyls[0], 0.8);
            Eigen::Vector3f translation{ 0, 0, 1.6};
            Eigen::Vector3f E = R + scene->locomotionCameras[i]->GetRotation() * translation;
            Eigen::Vector3f RE = (E - R).normalized();
            Eigen::Vector3f RD = (D - R).normalized();
            Eigen::Vector3f normal = RE.cross(RD);
            float dot = RD.dot(RE);
            if (abs(dot) > 1) dot = 1.0f;
            float theta = acos(dot);

            Eigen::Vector3f rotateAroundVector = scene->locomotionCameras[i]->GetAggregatedTransform().block<3, 3>(0, 0).inverse() * normal;
            scene->locomotionCameras[i]->Rotate(theta, rotateAroundVector);

            scene->locomotionCameras[i]->Translate(translate);

        }
    }

    void AnimationVisitor::rotateBackCameras()
    {
        for (int i = 0; i < scene->cameras.size(); i++)
        {
            auto system = scene->cameras[i]->GetRotation().transpose();
            auto angleCoeff = scene->cameras[i]->CalcAngleCoeff(1600);
            scene->cameras[i]->RotateInSystem(system, /*(-1*direction*directions[round])*/9.0 / angleCoeff, Movable::Axis::Y);
//            scene->cameras[i]->RotateInSystem(system, 0, Movable::Axis::X);
        }

    }



} // namespace cg3d
