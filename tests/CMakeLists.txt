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
rtype_add_test(NAME engine_tests SOURCES CollisionTest.cpp ComponentQueryTest.cpp ComponentTest.cpp EntityIndexMapTest.cpp EntityRegistryTest.cpp EventBusFixtures.cpp EventBusTest.cpp engine/Time/FixedTimestepTest.cpp NetworkEntityTableTest.cpp NetworkIdAllocatorTest.cpp SystemSchedulerTest.cpp TimerSchedulerTest.cpp LIBS rtype_engine TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
rtype_add_test(NAME concurrency_tests SOURCES concurrency/BoundedQueueTest.cpp LIBS rtype_concurrency TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
rtype_add_test(NAME game_tests SOURCES CollisionRulesTest.cpp CollisionScene.cpp CollisionSystemTest.cpp EntityTypeTest.cpp MovementSystemTest.cpp WorldTest.cpp LIBS rtype_game)
rtype_add_test(NAME network_tests SOURCES NetworkContextTest.cpp UdpSocketFixture.cpp UdpSocketTest.cpp ByteWriterTest.cpp ByteReaderTest.cpp SerializationRoundTripTest.cpp ByteSequence.cpp network/Transport/DatagramRelayTest.cpp network/Protocol/WorldStateCodecTest.cpp network/Protocol/WorldStateFixtures.cpp doubles/RecordingDatagramSocket.cpp LIBS rtype_network rtype_network_asio)
target_include_directories(network_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/doubles ${CMAKE_CURRENT_SOURCE_DIR}/network/Protocol)
rtype_add_test(NAME server_tests SOURCES server/Application/TickLoopTest.cpp server/Application/ServerApplicationTest.cpp LIBS rtype_server_application rtype_engine rtype_network_constants TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(server_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/server)
rtype_add_test(NAME server_replication_tests SOURCES server/Replication/WorldStateSenderTest.cpp server/Replication/WorldStateSenderFixture.cpp LIBS rtype_server_world_state_sender rtype_server_constants)
target_include_directories(server_replication_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/server/Replication)
rtype_add_test(NAME client_replication_tests SOURCES client/Replication/WorldStateFilterTest.cpp LIBS rtype_client_world_state_filter)
target_include_directories(client_replication_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Replication)
rtype_add_test(NAME server_platform_tests SOURCES server/Platform/FineTimerResolutionTest.cpp LIBS rtype_server_fine_timer_resolution)
rtype_add_test(NAME logging_tests SOURCES LoggerTest.cpp LogLaunchOptionsTest.cpp JournalFile.cpp LIBS rtype_logging)
rtype_add_test(NAME client_rendering_tests SOURCES RenderSystemTest.cpp client/Rendering/EntitySpritesTest.cpp client/Rendering/SpriteShowcaseTest.cpp LIBS rtype_client_entity_sprites rtype_client_render_system SFML::Graphics GTest::gmock)
target_compile_definitions(client_rendering_tests PRIVATE RTYPE_SOURCE_ASSETS_FOLDER="${PROJECT_SOURCE_DIR}/assets")
rtype_add_test(NAME client_tests SOURCES PlayfieldViewportTest.cpp WindowSizeSelectionTest.cpp client/Platform/ExecutablePathTest.cpp client/Assets/TemporaryFolder.cpp client/Assets/SilentSound.cpp client/Assets/AssetIdsTest.cpp client/Assets/AssetFolderTest.cpp client/Assets/AssetLibraryTest.cpp client/Assets/AssetLoadingStepTest.cpp LIBS rtype_client_playfield_viewport rtype_client_window_size_selection rtype_game rtype_client_asset_loading_step GTest::gmock)
rtype_add_test(NAME client_connection_tests SOURCES client/Connection/NetworkThreadTest.cpp LIBS rtype_client_network_thread TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(client_connection_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
rtype_add_test(NAME client_screen_tests SOURCES client/Application/ScreenStackTest.cpp client/Screens/GameScreenTest.cpp client/Screens/LoadingScreenTest.cpp client/Screens/ScreenAssetsTest.cpp client/Screens/RecordingScreen.cpp client/Screens/RecordingEntityLayer.cpp client/Screens/BlankRenderTarget.cpp client/Screens/KeyPress.cpp client/Assets/TemporaryFolder.cpp client/Assets/SilentSound.cpp LIBS rtype_client_screen_stack rtype_client_game_screen rtype_client_loading_screen GTest::gmock)
target_include_directories(client_screen_tests PRIVATE client/Assets client/Screens)
target_compile_definitions(client_screen_tests PRIVATE RTYPE_ASSETS_FOLDER="${CMAKE_SOURCE_DIR}/assets")
rtype_add_test(NAME client_background_tests SOURCES client/Rendering/Background/RecordingPixelSurface.cpp client/Rendering/Background/PixelBatchTest.cpp client/Rendering/Background/PlanetImageTest.cpp client/Rendering/Background/TailGradientTest.cpp client/Rendering/Background/SkyEventTest.cpp client/Rendering/Background/SkyEventSchedulerTest.cpp client/Rendering/Background/StarfieldTest.cpp client/Rendering/Background/IntegerScaleTest.cpp LIBS rtype_client_starfield rtype_client_sky_event_scheduler rtype_client_shooting_star rtype_client_flare rtype_client_asteroid rtype_client_comet rtype_client_planet_image rtype_client_pixel_batch rtype_client_integer_scale)
rtype_add_test(NAME client_input_tests SOURCES client/Input/KeyBindingsTest.cpp client/Input/ActionSetTest.cpp client/Input/PlayerCommandsTest.cpp LIBS rtype_client_player_commands)
