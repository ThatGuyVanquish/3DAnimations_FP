#pragma once
#include <GL.h>
#include "AnimationVisitor.h"
#include "Visitor.h"
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"


namespace cg3d
{

//    AnimationVisitor::AnimationVisitor(Scene *scene) : scene(scene) {}

    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {

        Visitor::Run(scene = _scene, camera);

    }

//
//    void AnimationVisitor::Visit(Scene* _scene)
//    {
//        Visitor::Visit(_scene); // draw children first
//
//        if (_scene->pickedModel && drawOutline)
//            DrawOutline();
//    }

    void AnimationVisitor::Visit(Model* model)
    {
        Eigen::Matrix3f system = model->GetRotation().transpose();
        Eigen::Vector3f rotation_x, rotation_y, rotation_z, rotation_vec;
        rotation_x = Eigen::Vector3f(1, 0, 0);
        rotation_y = Eigen::Vector3f(0, 1, 0);
        rotation_z = Eigen::Vector3f(0, 0, 1);

        if (scene->animate)
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
                    model->TranslateInSystem(system, Eigen::Vector3f(0, 0, -0.01f));
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
                if (prevRotatedCylIndex >= scene->numOfCyls-1)
                {
                    prevRotatedCylIndex = 0;
//                    doSkinning = true;
                }
            }
        }
        Visitor::Visit(model);
        //if (!model->isHidden) {
        //    Eigen::Matrix4f modelTransform = model->isStatic ? model->GetAggregatedTransform() : norm * model->GetAggregatedTransform();
        //    const Program* program = model->material->BindProgram();
        //    scene->Update(*program, proj, view, modelTransform);
        //    // glEnable(GL_LINE_SMOOTH);
        //    glLineWidth(model->lineWidth);

        //    // enable writing to the stencil only when we draw the picked model (and we want to draw an outline)
        //    glStencilMask(drawOutline && scene->pickedModel && model == scene->pickedModel.get() ? 0xFF : 0x0);

        //    model->material->BindProgram(); // call BindProgram() again to rebind the textures because igl bind_mesh messes them up
        //    model->UpdateDataAndDrawMeshes(*program, model->showFaces, model->showTextures);

        //    if (model->showWireframe) {
        //        program = model->material->BindFixedColorProgram();
        //        scene->Update(*program, proj, view, modelTransform);
        //        program->SetUniform4fv("fixedColor", 1, &model->wireframeColor);
        //        model->UpdateDataAndDrawMeshes(*program, false, false);
        //    }
        //}
    }


} // namespace cg3d
