cmake_minimum_required(VERSION 2.8.9)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME VirtualFractureReconstructionExtension)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/VirtualFractureReconstructionExtension")
  set(EXTENSION_CATEGORY "Examples")
  set(EXTENSION_CONTRIBUTORS "Karl Fritscher(BWH,MGH,UMIT), Peter Karasev (Georgia Tech)")
  set(EXTENSION_DESCRIPTION "This extension can be used to virtually reconstruct complex bone fractures")
  #set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/MyBarExtension/MyBarExtension.png?revision=21746&view=co")
  #set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-MyBarExtension-screenshot.png")
  set(EXTENSION_STATUS "Beta. Slicer version of fracture reconstruction pipeline. Due to restrictions for Slicer extensions (e.g. no CUDA support) functionality is restricted.")
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
add_subdirectory(VirtualFractureReconstructionCLI)
add_subdirectory(VirtualFractureReconstruction)
#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
