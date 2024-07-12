#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"
#include "Vertex.h"
#include "Pipeline.h"

namespace JzRE {

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(I32 w, I32 h) :
        deviceWidth(w), deviceHeight(h){};

    // Setting
    void SetViewMatrix(const Mat4x4 &view) {
        viewMatrix = view;
    }
    void SetModelMatrix(const Mat4x4 &model) {
        modelMatrix = model;
    }
    void SetProjectMatrix(const Mat4x4 &project, float nr, float fa) {
        projectMatrix = project;
        frustumNearFar = Vec2(nr, fa);
    }
    void SetViewerPos(const Vec3 &viewer);

    void BindFrambuffer(SharedPtr<Framebuffer> fb) {
        framebuffer = fb;
    }
    void BindPipeline(SharedPtr<Pipeline> pl) {
        pipeline = pl;
    }

    void DrawPixel(I32 x, I32 y, const Color &color);
    void DrawLine(I32 x1, I32 y1, I32 x2, I32 y2, const Color &c);
    void DrawPrimitive(Vertex v0, Vertex v1, Vertex v2);

private:
    I32 deviceWidth, deviceHeight;
    Mat4x4 viewMatrix;
    Mat4x4 modelMatrix;
    Mat4x4 projectMatrix;
    Vec2 frustumNearFar;

    SharedPtr<Framebuffer> framebuffer;
    SharedPtr<Pipeline> pipeline;
};

} // namespace JzRE