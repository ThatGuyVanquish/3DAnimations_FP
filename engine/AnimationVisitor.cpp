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
                    if ((time(nullptr) - scene->gameplay.timeFromLastWASDQE) > 0.5) {
                        snakyLocomotion();
                    } /*else {
                        round = 0;
                        direction = -1;
                    } */
                    model->TranslateInSystem(system, scene->gameplay.velocityVec);
                } else if (cylIndex == 1 && prevRotatedCylIndex == 0) {
                    rotateXY(model);
                } else if (cylIndex == prevRotatedCylIndex + 1) {
                    model->Rotate(prev_quat_x.conjugate());
                    model->Rotate(prev_quat_y.conjugate());
                    rotateXY(model);
                }

                if (prevRotatedCylIndex >= scene->gameplay.numOfCyls-1)
                {
                    std::cout << "timeFromLastWASDQE: " << (time(nullptr) - scene->gameplay.timeFromLastWASDQE) - 0.5 << std::endl;
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
        model->Rotate(prev_quat_x); //might need rotate in system
        rotation_vec = model->Tout.rotation() * rotation_y;
        prev_quat_y = Eigen::Quaternionf::FromTwoVectors(rotation_vec, rotation_y);
        prev_quat_y = prev_quat_y.slerp(scene->gameplay.slerpFactor, Eigen::Quaternionf::Identity());
        model->Rotate(prev_quat_y); //might need rotate in system
        prevRotatedCylIndex++;
    }

    void AnimationVisitor::snakyLocomotion()
    {

        scene->gameplay.cyls[0].model->Rotate(direction*directions[round], Movable::Axis::Y);
        scene->gameplay.cyls[1].model->Rotate(-1*direction*directions[round], Movable::Axis::Y);
        round += direction;
        if (round < 0 || round >= directions.size()) {
            direction *= -1;
            round += direction;
        }
//        scene->gameplay.cyls[0].model->Rotate(direction*0.1f, Movable::Axis::Y);
//        scene->gameplay.cyls[1].model->Rotate(-1*direction*0.1f, Movable::Axis::Y);
//        round++;
//        if ((round = (round % rounds)) == 0) {
//            if (first) {
//                rounds *= 2;
//                first = false;
//            }
//            direction *= -1;
//        }
    }



} // namespace cg3d
