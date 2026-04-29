cmake_minimum_required(VERSION 3.12)
project(CylinderExample)

find_package(VTK REQUIRED COMPONENTS
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

add_executable(CylinderExample CylinderExample.cxx)
target_link_libraries(CylinderExample PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
  TARGETS CylinderExample
  MODULES ${VTK_LIBRARIES}
)