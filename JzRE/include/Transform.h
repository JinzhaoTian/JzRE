#pragma once

#include "CommonTypes.h"
#include "Vector.h"
#include "Matrix.h"

namespace JzRE {
class Transform {
public:
    void Translate(const Vec3 &vector);
    void Rotate(float angle, const Vec3 &axis);
    void Scale(const Vec3 &factor);

private:
    Mat4 matrix;
};
} // namespace JzRE