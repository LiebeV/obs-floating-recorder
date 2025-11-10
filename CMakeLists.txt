cmake_minimum_required(VERSION 3.16)
project(obs-floating-recorder VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

# OBS路径设置
set(OBS_INSTALL_PATH "$ENV{OBS_INSTALL_PATH}")
if(NOT OBS_INSTALL_PATH)
    set(OBS_INSTALL_PATH "${CMAKE_SOURCE_DIR}/obs-install/obs-studio")
endif()

set(OBS_LIBRARY_PATH "${OBS_INSTALL_PATH}/bin/64bit")
set(OBS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/obs-src/obs-studio-28.1.2/libobs")
set(OBS_FRONTEND_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/obs-src/obs-studio-28.1.2/UI/obs-frontend-api")

message(STATUS "OBS Library Path: ${OBS_LIBRARY_PATH}")
message(STATUS "OBS Include Dir: ${OBS_INCLUDE_DIR}")

# 设置自动moc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 添加插件源文件 - 使用变量以便后续处理
set(SOURCES
    src/floating-recorder-plugin.cpp
)

# 创建插件目标
add_library(obs-floating-recorder MODULE ${SOURCES})

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
    # 添加必要的定义
    target_compile_definitions(obs-floating-recorder PRIVATE
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
    
    # 设置库文件路径
    link_directories("${OBS_LIBRARY_PATH}")
    
    # 链接OBS库
    target_link_libraries(obs-floating-recorder
        obs
        obs-frontend-api
    )
    
    # 复制必要的DLL文件到输出目录
    add_custom_command(TARGET obs-floating-recorder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OBS_LIBRARY_PATH}/obs.dll"
            $<TARGET_FILE_DIR:obs-floating-recorder>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OBS_LIBRARY_PATH}/obs-frontend-api.dll"
            $<TARGET_FILE_DIR:obs-floating-recorder>
        COMMENT "Copying OBS DLL dependencies"
    )
endif()

# 设置输出属性
set_target_properties(obs-floating-recorder PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
    AUTOMOC ON
)
