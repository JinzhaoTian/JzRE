#pragma once

#include "CommonTypes.h"
#include "Shader.h"

namespace JzRE {

enum EShaderStage {
    ShaderStage_Vertex,
    ShaderStage_Fragment
};

class Pipeline {
public:
    struct FragmentData;

    struct VertexData {
        Vec3 m_pos;   // World space position
        Vec3 m_nor;   // World space normal
        Vec2 m_tex;   // World space texture coordinate
        Vec4 m_cpos;  // Clip space position
        IVec2 m_spos; // Screen space position
        // glm::mat3 m_tbn;  //Tangent, bitangent, normal matrix
        Bool m_needInterpolatedTBN = false;
        F32 m_rhw;

        VertexData() = default;
        VertexData(const IVec2 &screenPos) :
            m_spos(screenPos) {
        }

        // Linear interpolation
        static VertexData lerp(const VertexData &v0, const VertexData &v1, float frac);

        static FragmentData barycentricLerp(const VertexData &v0, const VertexData &v1, const VertexData &v2, const Vec3 &w);
        static float barycentricLerp(const float &d0, const float &d1, const float &d2, const Vec3 &w);

        // Perspective correction for interpolation
        static void prePerspCorrection(VertexData &v);
    };

    struct FragmentData {
    public:
        Vec3 m_pos;   // World space position
        Vec3 m_nor;   // World space normal
        Vec2 m_tex;   // World space texture coordinate
        IVec2 m_spos; // Screen space position
        // glm::mat3 m_tbn;  //Tangent, bitangent, normal matrix
        F32 m_rhw;

        FragmentData() = default;
        FragmentData(const IVec2 &screenPos) :
            m_spos(screenPos) {
        }

        static void aftPrespCorrection(FragmentData &v);
    };

public:
    Pipeline() = default;
    virtual ~Pipeline() = default;

    // Shaders
    virtual void VertShader(VertexData &vertex) const = 0;
    virtual void FragShader(const FragmentData &data, Vec4 &fragColor) const = 0;

private:
};

} // namespace JzRE