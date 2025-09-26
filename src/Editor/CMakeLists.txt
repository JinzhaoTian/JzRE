add_library(JzEditor STATIC)

target_sources(JzEditor PRIVATE
    JzEditor.cpp
    JzHierarchy.cpp
    JzAssetBrowser.cpp # ...
)

target_include_directories(JzEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编辑器模块依赖渲染、场景、资源和 UI

target_link_libraries(JzEditor
    PUBLIC
    JzRenderOpenGL # 或者只暴露 JzRHI，具体实现设为 PRIVATE
    JzScene
    JzResource
    JzUI
    PRIVATE
    JzCore
)