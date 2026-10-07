cmake_minimum_required(VERSION 3.16)
project(VMS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Tipo de compilacion" FORCE)
endif()

set(GLEW_DIR   ${CMAKE_SOURCE_DIR}/external/glew-2.3.1)
set(IMGUI_DIR  ${CMAKE_SOURCE_DIR}/external/imgui)
set(BCRYPT_DIR ${CMAKE_SOURCE_DIR}/libbcrypt)

foreach(required_path
        ${CMAKE_SOURCE_DIR}/external/glfw-3.5.1/CMakeLists.txt
        ${GLEW_DIR}/include/GL/glew.h
        ${IMGUI_DIR}/imgui.cpp)
    if(NOT EXISTS ${required_path})
        message(FATAL_ERROR "Falta ${required_path}. Revisa la carpeta external/.")
    endif()
endforeach()

file(GLOB GLEW_LIBS ${GLEW_DIR}/lib/libGLEW.*)
if(NOT GLEW_LIBS)
    message(FATAL_ERROR "GLEW no esta compilado: ejecuta 'make' dentro de ${GLEW_DIR}")
endif()

find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libswscale libavutil)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.5.1)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(${BCRYPT_DIR})

add_executable(vms
    src/main.cpp
    src/core/Application.cpp
    src/core/styles/Theme.cpp
    src/core/media/VideoPlayer.cpp
    src/config/connection/Database.cpp
    src/controllers/AuthController.cpp
    src/repositories/AuthRepository.cpp
    src/security/PasswordHasher.cpp
    src/session/Session.cpp
    src/services/auth/AuthService.cpp
    src/views/LoginWindow.cpp
    src/views/VMSWindow.cpp
    src/components/Sidebar.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(vms PRIVATE
    src
    ${GLEW_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${MYSQL_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${BCRYPT_DIR}/include
)

target_compile_options(vms PRIVATE ${MYSQL_CFLAGS_OTHER} ${FFMPEG_CFLAGS_OTHER})

target_link_directories(vms PRIVATE
    ${GLEW_DIR}/lib
    ${MYSQL_LIBRARY_DIRS}
    ${FFMPEG_LIBRARY_DIRS}
)

target_link_libraries(vms PRIVATE
    GLEW
    glfw
    OpenGL::GL
    ${MYSQL_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    bcrypt
)

set_target_properties(vms PROPERTIES BUILD_RPATH ${GLEW_DIR}/lib)