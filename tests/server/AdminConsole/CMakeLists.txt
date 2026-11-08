if(NOT RTYPE_MODULE_ADMIN_CONSOLE)
  return()
endif()

add_subdirectory(Input)
add_subdirectory(Typing)
add_subdirectory(Output)
add_subdirectory(Commands)

target_sources(server_module_tests PRIVATE ServerAdminConsoleModuleFixture.cpp ServerAdminConsoleModuleTest.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/ScriptedConsoleInput.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingConsoleOutput.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingTerminal.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/ThrowingTerminal.cpp ${RTYPE_TEST_SOURCE_DIR}/server/Network/Rejections/RejectionsFixtures.cpp)

target_include_directories(server_module_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/server/Network/Rejections)

target_link_libraries(server_module_tests PRIVATE rtype_server_admin_console_module)
