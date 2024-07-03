#pragma once

#include "CommonTypes.h"
#include "Color.h"
#include "Vector.h"
#include "Matrix.h"

namespace JzRE {

class Vertex {
public:
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
    Color color;

public:
    inline Vertex() :
        position(nullptr), normal(nullptr), texCoords(nullptr), color(Color()) {
    }
    inline Vertex(const Vec3 &p, const Color &c, Vec2 uv) :
        position(p), texCoords(uv), color(c), normal(nullptr) {
    }
    inline Vertex(const Vec3 &p, const Color &c, const Vec3 &normal, const Vec2 &uv) :
        position(p), texCoords(uv), color(c), normal(normal) {
    }
    ~Vertex() {
    }

    // void LerpVertexData(Vertex& left, const Vertex& right, float t);
    // void Print();
    // Vertex& operator * (const Matrix& m);
    // static float LerpFloat(float v1, float v2, float t) { return v1 + (v2 - v1) * t; }
};

} // namespace JzRE