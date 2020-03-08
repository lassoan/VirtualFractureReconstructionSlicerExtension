cmake_minimum_required(VERSION 3.13.4)

project(VirtualFractureReconstruction)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/VirtualFractureReconstruction")
set(EXTENSION_CATEGORY "IGT")
set(EXTENSION_CONTRIBUTORS "Karl Fritscher(BWH,MGH,UMIT), Peter Karasev (Georgia Tech)")
set(EXTENSION_DESCRIPTION "This extension can be used to virtually reconstruct complex bone fractures")
set(EXTENSION_ICONURL "http://www.slicer.org/slicerWiki/images/5/5a/Fracture_icon.png")
set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/4/4c/VirtualFractureReconstruction_Screen.png")
set(EXTENSION_STATUS "Beta. Slicer version of fracture reconstruction pipeline. Due to restrictions for Slicer extensions (e.g. no CUDA support) functionality is restricted.")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(VirtualFractureReconstructionCLI)
add_subdirectory(VirtualFractureReconstruction)
## NEXT_MODULE

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
