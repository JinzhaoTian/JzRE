set(TARGET_NAME JzREShaderTool)

file(GLOB_RECURSE JzREShaderTool_HEADERS CONFIGURE_DEPENDS "source/*.h")
file(GLOB_RECURSE JzREShaderTool_SOURCES CONFIGURE_DEPENDS "source/*.cpp")

add_executable(
    ${TARGET_NAME}
    ${JzREShaderTool_HEADERS}
    ${JzREShaderTool_SOURCES}
)

target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(
    ${TARGET_NAME}
    PRIVATE
    nlohmann_json::nlohmann_json
)
