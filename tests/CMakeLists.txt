find_package(GTest CONFIG REQUIRED)
find_package(SFML 3 COMPONENTS Graphics Audio CONFIG REQUIRED)

include(GoogleTest)

set(RTYPE_THREAD_TEST_TIMEOUT_SECONDS 60)

function(rtype_add_test)
  set(oneValueArgs NAME TIMEOUT)
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

  if(ARG_TIMEOUT)
    gtest_discover_tests(${ARG_NAME} PROPERTIES TIMEOUT ${ARG_TIMEOUT})
  else()
    gtest_discover_tests(${ARG_NAME})
  endif()
endfunction()

rtype_add_test(NAME harness_tests SOURCES HarnessTest.cpp)
rtype_add_test(NAME engine_tests SOURCES ComponentQueryTest.cpp ComponentTest.cpp EntityIndexMapTest.cpp EntityRegistryTest.cpp EventBusFixtures.cpp EventBusTest.cpp FixedTimestepTest.cpp SystemSchedulerTest.cpp TimerSchedulerTest.cpp engine/Concurrency/BoundedQueueTest.cpp LIBS rtype_engine TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
rtype_add_test(NAME game_tests SOURCES EntityTypeTest.cpp MovementSystemTest.cpp WorldTest.cpp LIBS rtype_game)
rtype_add_test(NAME network_tests SOURCES NetworkContextTest.cpp UdpSocketFixture.cpp UdpSocketTest.cpp ByteWriterTest.cpp ByteReaderTest.cpp SerializationRoundTripTest.cpp ByteSequence.cpp LIBS rtype_network rtype_network_asio)
rtype_add_test(NAME server_tests SOURCES server/Application/TickLoopTest.cpp server/Application/DatagramRelayTest.cpp server/Application/ServerApplicationTest.cpp doubles/RecordingDatagramSocket.cpp LIBS rtype_server_application rtype_engine rtype_network_constants TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(server_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/doubles ${CMAKE_CURRENT_SOURCE_DIR}/server)
rtype_add_test(NAME server_platform_tests SOURCES server/Platform/FineTimerResolutionTest.cpp LIBS rtype_server_fine_timer_resolution)
rtype_add_test(NAME logging_tests SOURCES LoggerTest.cpp LogLaunchOptionsTest.cpp JournalFile.cpp LIBS rtype_logging)
rtype_add_test(NAME client_rendering_tests SOURCES RenderSystemTest.cpp LIBS rtype_client_render_system)
rtype_add_test(NAME client_tests SOURCES PlayfieldViewportTest.cpp WindowSizeSelectionTest.cpp client/Platform/ExecutablePathTest.cpp client/Assets/TemporaryFolder.cpp client/Assets/SilentSound.cpp client/Assets/AssetIdsTest.cpp client/Assets/AssetFolderTest.cpp client/Assets/AssetLibraryTest.cpp client/Assets/AssetLoadingStepTest.cpp LIBS rtype_client_playfield_viewport rtype_client_window_size_selection rtype_game rtype_client_asset_loading_step GTest::gmock)
rtype_add_test(NAME client_background_tests SOURCES client/Rendering/Background/RecordingPixelSurface.cpp client/Rendering/Background/PixelBatchTest.cpp client/Rendering/Background/PlanetImageTest.cpp client/Rendering/Background/TailGradientTest.cpp client/Rendering/Background/SkyEventTest.cpp client/Rendering/Background/SkyEventSchedulerTest.cpp client/Rendering/Background/StarfieldTest.cpp client/Rendering/Background/IntegerScaleTest.cpp LIBS rtype_client_starfield rtype_client_sky_event_scheduler rtype_client_shooting_star rtype_client_flare rtype_client_asteroid rtype_client_comet rtype_client_planet_image rtype_client_pixel_batch rtype_client_integer_scale)
