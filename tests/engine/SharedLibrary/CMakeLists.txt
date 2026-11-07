if(NOT RTYPE_MODULE_PLUGINS)
  return()
endif()

target_sources(engine_tests PRIVATE SharedLibraryTest.cpp)
