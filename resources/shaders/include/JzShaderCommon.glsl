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
#define JZ_VK_LAYOUT_LOCATION(locationIndex) layout(location = locationIndex)
#else
#define JZ_FLIP_CLIP_Y(position)
#define JZ_VK_LAYOUT_LOCATION(locationIndex)
#endif

#endif // JZ_SHADER_COMMON_GLSL
