#include <GL.h>
#include "AnimationVisitor.h"
#include "Visitor.h"
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"


namespace cg3d
{
    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        if (drawOutline) {
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
        }

        Visitor::Run(scene = _scene, camera);
    }

    void AnimationVisitor::Init()
    {
        // clear and set up the depth and color buffers (and the stencil buffer if outline is enabled)
        unsigned int flags = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glClearColor(0, 0, 0, 0);

        // clear and set up the stencil buffer if outline is enabled
        if (drawOutline) {
            glStencilMask(0xFF);
            glClearStencil(0);
            flags |= GL_STENCIL_BUFFER_BIT;
        }

        glClear(flags);
    }

    void AnimationVisitor::Visit(Scene* _scene)
    {
        Visitor::Visit(_scene); // draw children first

        if (_scene->pickedModel && drawOutline)
            DrawOutline();
    }

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
                vec2 = model->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
                Eigen::Quaternionf quat = Eigen::Quaternionf::FromTwoVectors(vec2, vec1);
                quat = quat.slerp(0.75, Eigen::Quaternionf::Identity());
                model->Rotate(quat); //might need rotate in system
                int cylIndex = std::stoi(modelsName.substr(4));
                if (cylIndex == 0) // need to check what's the head of the snake
                {
                    model->TranslateInSystem(system, Eigen::Vector3f(-0.01f, 0, 0));
                }
                Visitor::Visit(model);
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

    void AnimationVisitor::DrawOutline()
    {
        auto& model = scene->pickedModel;
        auto program = model->material->BindFixedColorProgram();
        scene->Update(*program, proj, view, model->isStatic ? model->GetAggregatedTransform() : norm * model->GetAggregatedTransform());
        program->SetUniform4fv("fixedColor", 1, &outlineLineColor);

        // draw the picked model with thick lines only where previously the stencil wasn't touched (i.e. around the original model)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilMask(0x0);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glLineWidth(outlineLineWidth);
        model->UpdateDataAndDrawMeshes(*program, false, false);
    }

} // namespace cg3d
