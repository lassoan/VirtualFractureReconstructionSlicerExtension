project(vtkSlicer${qt_module_name}ModuleMRML)

#
# See CMake/SlicerMacroBuildModuleMRML.cmake for details
#

set(module_mrml_name "${PROJECT_NAME}")

# The header '${module_mrml_name}Export.h' will be automatically configured.
set(module_mrml_export_directive "Q_SLICER_QTMODULES_VIRTUALFRACTURERECONSTRUCTION_EXPORT")

# Additional directories to include
set(module_mrml_include_directories
  )

# Source files
set(module_mrml_SRCS
  vtkMRML${qt_module_name}Node.cxx
  vtkMRML${qt_module_name}Node.h
  vtkMRMLCLIFractureReconstructionPropertyNode.cxx
  vtkMRMLCLIFractureReconstructionPropertyNode.h
  )

# Additional Target libraries
set(module_mrml_target_libraries
  ${ITK_LIBRARIES}
  ${MRML_LIBRARIES}
  )

SlicerMacroBuildModuleMRML(
  NAME ${module_mrml_name}
  EXPORT_DIRECTIVE ${module_mrml_export_directive}
  INCLUDE_DIRECTORIES ${module_mrml_include_directories}
  SRCS ${module_mrml_SRCS}
  TARGET_LIBRARIES ${module_mrml_target_libraries}
  )
