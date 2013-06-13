# This project is designed to be built outside the Insight source tree.
cmake_minimum_required(VERSION 2.8.8)

#-----------------------------------------------------------------------------
set(MODULE_NAME FractureReconstructionCLI)
#set(MODULE_TITLE ${MODULE_NAME})

#string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
#if(NOT Slicer_SOURCE_DIR)
#  find_package(Slicer REQUIRED)
#  include(${Slicer_USE_FILE})
#endif()

#if(NOT Slicer_SOURCE_DIR)
#  include(${Slicer_EXTENSION_CPACK})
#endif()

#-----------------------------------------------------------------------------
#set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

#-----------------------------------------------------------------------------

add_subdirectory(Core)
