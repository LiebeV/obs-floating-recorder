cmake_minimum_required(VERSION 3.16)
project(obs-floating-recorder VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

# 设置OBS路径
set(OBS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/obs-src/obs-studio-28.1.2/libobs")
set(OBS_FRONTEND_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/obs-src/obs-studio-28.1.2/UI/obs-frontend-api")

# 设置UI文件
set(UI_FILES src/floating-window.ui)

# 添加插件源文件
add_library(obs-floating-recorder MODULE
    src/floating-recorder-plugin.cpp
    src/floating-window.cpp
    src/floating-window.h
    ${UI_FILES}
)

# 自动处理Qt的moc和ui文件
qt5_wrap_cpp(MOC_SOURCES src/floating-window.h)
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

target_sources(obs-floating-recorder PRIVATE 
    ${MOC_SOURCES}
    ${UI_HEADERS}
)

# 包含目录
target_include_directories(obs-floating-recorder PRIVATE
    ${OBS_INCLUDE_DIR}
    ${OBS_FRONTEND_INCLUDE_DIR}
    src/
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 链接库
target_link_libraries(obs-floating-recorder
    Qt5::Widgets
    Qt5::Core
)

# Windows特定设置
if(WIN32)
    target_link_libraries(obs-floating-recorder
        ${CMAKE_SOURCE_DIR}/deps/lib/obs.lib
        ${CMAKE_SOURCE_DIR}/deps/lib/obs-frontend-api.lib
    )
    
    # 添加必要的定义
    target_compile_definitions(obs-floating-recorder PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# 设置输出属性
set_target_properties(obs-floating-recorder PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)
