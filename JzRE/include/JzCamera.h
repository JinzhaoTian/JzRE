#pragma once

#include "CommonTypes.h"
#include "JzEntity.h"

namespace JzRE {
class JzCamera : public JzEntity {
public:
    JzCamera();

    void SetFov(F32 p_value);

    F32 GetFov() const;

    void SetFar(F32 p_value);

    F32 GetFar() const;

    void SetClearColor();

private:
    F32 m_fov;
    F32 m_size;
    F32 m_near;
    F32 m_far;
};
} // namespace JzRE