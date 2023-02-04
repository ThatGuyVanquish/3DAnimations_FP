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
        Eigen::Vector3f vec1, vec2;
        vec1 = Eigen::Vector3f(1, 0, 0);
        Visitor::Visit(model); // draw children first
        if (scene->animate)
        {
            std::string modelsName = model->name;
            if (modelsName.starts_with("Cyl"))
            {
                int cylIndex = std::stoi(modelsName.substr(4));
                if (cylIndex == 0) // need to check what's the head of the snake
                {
                    model->TranslateInSystem(system, Eigen::Vector3f(-0.01f, 0, 0));
                }
                else {
                    if (cylIndex == 1) {
                        vec2 = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        prevRotationQuaternion = Eigen::Quaternionf::FromTwoVectors(vec2, vec1);
                        prevRotationQuaternion = prevRotationQuaternion.slerp(0.99, Eigen::Quaternionf::Identity());
                        model->Rotate(prevRotationQuaternion); //might need rotate in system
                    } else {
                        model->Rotate(prevRotationQuaternion.conjugate());
                        vec2 = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                        prevRotationQuaternion = Eigen::Quaternionf::FromTwoVectors(vec2, vec1);
                        prevRotationQuaternion = prevRotationQuaternion.slerp(0.99, Eigen::Quaternionf::Identity());
                        model->Rotate(prevRotationQuaternion); //might need rotate in system
                    }
                }
//                Visitor::Visit(model);
            }
        }
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
