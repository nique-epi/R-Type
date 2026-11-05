# The plugins the tests load: shared libraries built next to the test
# executable, in plugins/. Each links the static libraries of the engine it
# uses, as a plugin of the server does.
rtype_add_plugin(TARGET rtype_test_plugin_probe NAME probe
  SOURCES ProbePluginEntry.cpp ProbePluginModule.cpp
  LIBS rtype_engine_event_bus)
rtype_add_plugin(TARGET rtype_test_plugin_entryless NAME entryless
  SOURCES EntrylessPlugin.cpp WITHOUT_ENTRY)
rtype_add_plugin(TARGET rtype_test_plugin_future NAME future
  SOURCES FuturePluginEntry.cpp)

foreach(plugin rtype_test_plugin_probe rtype_test_plugin_entryless rtype_test_plugin_future)
  target_include_directories(${plugin} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles)
  set_target_properties(${plugin} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${RTYPE_TEST_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${RTYPE_TEST_BINARY_DIR}/plugins)
endforeach()

# A MODULE library is linked by nothing: the test that loads it waits for it.
add_dependencies(engine_tests rtype_test_plugin_probe rtype_test_plugin_entryless rtype_test_plugin_future)

# The tests read where the plugins are and which test file names them.
target_sources(engine_tests PRIVATE PluginFiles.cpp)
target_include_directories(engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
