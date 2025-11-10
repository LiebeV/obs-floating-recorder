cmake_minimum_required(VERSION 3.16)
project(obs-floating-recorder VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

# 在GitHub Actions中，我们假设OBS开发包已安装
find_path(OBS_INCLUDE_DIR NAMES obs/obs.h)
find_path(OBS_FRONTEND_INCLUDE_DIR NAMES obs-frontend-api/obs-frontend-api.h)

if(NOT OBS_INCLUDE_DIR)
    message(FATAL_ERROR "OBS include directory not found")
endif()

if(NOT OBS_FRONTEND_INCLUDE_DIR)
    message(FATAL_ERROR "OBS Frontend include directory not found")
endif()

# 添加插件源文件
add_library(obs-floating-recorder MODULE
    src/floating-recorder-plugin.cpp
    src/floating-window.cpp
)

# 自动处理Qt的moc
target_link_libraries(obs-floating-recorder
    Qt5::Widgets
    Qt5::Core
)

target_include_directories(obs-floating-recorder PRIVATE
    ${OBS_INCLUDE_DIR}
    ${OBS_FRONTEND_INCLUDE_DIR}
    src/
)

if(WIN32)
    target_link_libraries(obs-floating-recorder
        ${OBS_INCLUDE_DIR}/../bin/64bit/obs.lib
        ${OBS_INCLUDE_DIR}/../bin/64bit/obs-frontend-api.lib
    )
endif()

set_target_properties(obs-floating-recorder PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

if(WIN32)
    set_target_properties(obs-floating-recorder PROPERTIES SUFFIX ".dll")
endif()