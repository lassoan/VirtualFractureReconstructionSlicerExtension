 
set(KIT qSlicer${qt_module_name}Module)

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS
  qSlicer${qt_module_name}ModuleWidgetTest.cxx
  )

#-----------------------------------------------------------------------------
QT4_GENERATE_MOCS(
  qSlicer${qt_module_name}ModuleWidgetTest.cxx
  )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  WITH_VTK_DEBUG_LEAKS_CHECK
  )

#-----------------------------------------------------------------------------
simple_test(qSlicer${qt_module_name}ModuleWidgetTest)