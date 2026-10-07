find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

function(rtype_add_test)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES LIBS)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_NAME)
    message(FATAL_ERROR "rtype_add_test: NAME is required")
  endif()
  if(NOT ARG_SOURCES)
    message(FATAL_ERROR "rtype_add_test(${ARG_NAME}): SOURCES is required")
  endif()

  add_executable(${ARG_NAME} ${ARG_SOURCES})

  target_link_libraries(${ARG_NAME}
    PRIVATE
      GTest::gtest_main
      ${ARG_LIBS}
  )

  set_target_properties(${ARG_NAME}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )

  rtype_enable_warnings(${ARG_NAME})

  gtest_discover_tests(${ARG_NAME})
endfunction()

rtype_add_test(NAME harness_tests SOURCES HarnessTest.cpp)
rtype_add_test(NAME engine_tests SOURCES ComponentQueryTest.cpp ComponentTest.cpp EntityIndexMapTest.cpp EntityRegistryTest.cpp EventBusFixtures.cpp EventBusTest.cpp FixedTimestepTest.cpp SystemSchedulerTest.cpp TimerSchedulerTest.cpp LIBS rtype_engine)
rtype_add_test(NAME game_tests SOURCES EntityTypeTest.cpp MovementSystemTest.cpp WorldTest.cpp LIBS rtype_game)
rtype_add_test(NAME network_tests SOURCES NetworkContextTest.cpp UdpSocketFixture.cpp UdpSocketTest.cpp ByteWriterTest.cpp ByteReaderTest.cpp SerializationRoundTripTest.cpp ByteSequence.cpp LIBS rtype_network rtype_network_asio)
rtype_add_test(NAME logging_tests SOURCES LoggerTest.cpp LogLaunchOptionsTest.cpp JournalFile.cpp LIBS rtype_logging)
rtype_add_test(NAME client_rendering_tests SOURCES RenderSystemTest.cpp LIBS rtype_client_render_system)
rtype_add_test(NAME client_tests SOURCES PlayfieldViewportTest.cpp WindowSizeSelectionTest.cpp LIBS rtype_client_playfield_viewport rtype_client_window_size_selection rtype_game)
