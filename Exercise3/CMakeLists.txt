cmake_minimum_required(VERSION 3.19)
project(Worksheet6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)
qt_standard_project_setup()

# VTK
set(VTK_DIR "C:/Program Files (x86)/VTK/lib/cmake/vtk-9.6")

# OpenVR - required by VTK RenderingOpenVR
set(OpenVR_INCLUDE_DIR "D:/OpenVR/headers" CACHE PATH "OpenVR include directory" FORCE)
set(OpenVR_LIBRARY "d:/OpenVR/lib/win64/openvr_api.lib" CACHE FILEPATH "OpenVR library" FORCE)
set(OpenVR_BIN_DIR "D:/OpenVR/bin/win64" CACHE PATH "OpenVR bin directory" FORCE)


find_package(VTK REQUIRED COMPONENTS
CommonColor
  CommonCore
  CommonDataModel
  FiltersSources
  FiltersGeneral
  InteractionStyle
  RenderingCore
  RenderingOpenGL2
  RenderingQt
  GUISupportQt
  IOGeometry
  IOLegacy
  RenderingOpenVR
)

qt_add_executable(Worksheet6
  WIN32
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  ModelPart.cpp
  ModelPart.h
  ModelPartList.cpp
  ModelPartList.h
  optiondialog.cpp
  optiondialog.h
  optiondialog.ui
  resources.qrc
  VRRenderThread.cpp
VRRenderThread.h
)

target_link_libraries(Worksheet6 PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::OpenGLWidgets
  ${VTK_LIBRARIES}
  ${OpenVR_LIBRARY}
)

vtk_module_autoinit(
  TARGETS Worksheet6
  MODULES ${VTK_LIBRARIES}
)

add_custom_target(VRBindings)

add_custom_command(TARGET VRBindings PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E
  copy_directory ${CMAKE_SOURCE_DIR}/vrbindings ${CMAKE_BINARY_DIR}/
)

add_custom_command(TARGET Worksheet6 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${OpenVR_BIN_DIR}/openvr_api.dll"
  $<TARGET_FILE_DIR:Worksheet6>
)