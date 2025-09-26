add_executable(JzRE
    JzREHub.cpp
)

# 链接编辑器库，它会传递所有其他需要的依赖

target_link_libraries(JzRE
    PRIVATE
    JzEditor
)