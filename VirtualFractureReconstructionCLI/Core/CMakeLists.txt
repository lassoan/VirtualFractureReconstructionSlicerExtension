# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
        INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF(VTK_FOUND)

#Disable some warning since some unused functions/parameters are left in the code intentionally for later modifications


set(MODULE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/MeanCurvatureCalculator
        ${CMAKE_CURRENT_SOURCE_DIR}/VTK
        ${CMAKE_CURRENT_SOURCE_DIR}/Transform
        ${CMAKE_CURRENT_SOURCE_DIR}/ImageRegistration
        ${CMAKE_CURRENT_SOURCE_DIR}/Sheetness
        ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/Statistics
        ${CMAKE_CURRENT_SOURCE_DIR}/ICP
        ${CMAKE_CURRENT_SOURCE_DIR}/memini
        ${CMAKE_CURRENT_SOURCE_DIR}/Texture
        ${CMAKE_CURRENT_SOURCE_DIR}/Mahalanobis
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} BaseSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Texture TextureSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Transform TransformSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/MeanCurvatureCalculator MeanCurvatureCalculatorSources)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Utilities UtilitiesSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Statistics StatisticsSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Sheetness SheetnessSources)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/memini MeminiSources)


set(MODULE_SRCS

        ${BaseSources}
        ${TextureSources}
        ${UtilitiesSources}
        ${TransformSources}
        ${MeanCurvatureCalculatorSources}
        ${StatisticsSources}
        ${MeminiSources}
        ${SheetnessSources}
        ${ALGLIBSources}

        ${CMAKE_CURRENT_SOURCE_DIR}/ICP/ICPWrapper.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/FractureReconstructionCLI.cxx
)

#FIND_LIBRARY(LAPACK_LIBRARY liblapack.so.3)
#FIND_LIBRARY(BLAS_LIBRARY libblas.so.3)

set(MODULE_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES} vtkHybrid vtkIO vtkRendering vtkCommon vtkFiltering vtkWidgets vtkVolumeRendering
  #${LAPACK_LIBRARY} ${BLAS_LIBRARY}
)


#-----------------------------------------------------------------------------

SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  ADDITIONAL_SRCS ${MODULE_SRCS}
  )








