#ifndef JZ_SHADER_COMMON_GLSL
#define JZ_SHADER_COMMON_GLSL

#ifndef JZ_BACKEND_OPENGL
#define JZ_BACKEND_OPENGL 0
#endif

#ifndef JZ_BACKEND_VULKAN
#define JZ_BACKEND_VULKAN 0
#endif

#if JZ_BACKEND_VULKAN
#define JZ_FLIP_CLIP_Y(position) (position).y = -(position).y
#else
#define JZ_FLIP_CLIP_Y(position)
#endif

#endif // JZ_SHADER_COMMON_GLSL
