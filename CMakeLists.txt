cmake_minimum_required(VERSION 3.2)

#################################
#Project
#################################
project(waterman)
message(STATUS "*** PROJECT_NAME: ${PROJECT_NAME}")

#################################
#Cxx standard
#################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++17 support has been enabled by default.")

#################################
#Set modulepath for find_package
#################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message(STATUS "*** CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "*** CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

#################################
#Executable
#################################
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simpleGeometryCollection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/waterman_sphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui/imgui.cpp
)

#################################
#Find opengl
#################################
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "*** Found OpenGl")
   	target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})
endif(OpenGL_FOUND)

#################################
#   Submodule glew (static)
#################################
add_definitions(-DGLEW_STATIC)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodule/glew-cmake EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} libglew_static)

#################################
#   Submodule glfw
# [Warning] This needs to be build AFTER OpenCv (Both define the target "uninstall")
# [Info] glfw CMakeLists.txt has: if (NOT TARGET uninstall)
# [Info] If glfw3.dll not found: Copy ...\build\submodule\glfw\src\Release\glfw3.dll
#################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodule/glfw)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/submodule/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

#################################
#Add subdirectory glm
#################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodule/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/submodule/glm)
target_link_libraries(${PROJECT_NAME} glm)

#################################
#Add subdirectory qhull
#https://github.com/yuki-koyama/easy-qhull
#################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodule/easy-qhull)
target_link_libraries(${PROJECT_NAME} qhull)