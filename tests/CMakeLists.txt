find_package(GTest CONFIG REQUIRED)
find_package(SFML 3 COMPONENTS Graphics Audio CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include(GoogleTest)

set(RTYPE_TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RTYPE_TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_library(rtype_test_support STATIC CurrentDirectoryChange.cpp DebugJournal.cpp JournalFile.cpp TemporaryFolder.cpp)
target_include_directories(rtype_test_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_test_support PUBLIC GTest::gtest rtype_logging)
rtype_enable_warnings(rtype_test_support)

set(RTYPE_THREAD_TEST_TIMEOUT_SECONDS 60)

function(rtype_add_test)
  set(oneValueArgs NAME TIMEOUT)
  set(multiValueArgs SOURCES LIBS)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_NAME)
    message(FATAL_ERROR "rtype_add_test: NAME is required")
  endif()

  add_executable(${ARG_NAME} ${ARG_SOURCES})

  target_link_libraries(${ARG_NAME}
    PRIVATE
      GTest::gtest_main
      ${ARG_LIBS}
  )

  set_target_properties(${ARG_NAME}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${RTYPE_TEST_BINARY_DIR}
  )

  rtype_enable_warnings(${ARG_NAME})

  if(ARG_TIMEOUT)
    gtest_discover_tests(${ARG_NAME} WORKING_DIRECTORY ${RTYPE_TEST_BINARY_DIR} PROPERTIES TIMEOUT ${ARG_TIMEOUT})
  else()
    gtest_discover_tests(${ARG_NAME} WORKING_DIRECTORY ${RTYPE_TEST_BINARY_DIR})
  endif()
endfunction()

rtype_add_test(NAME harness_tests SOURCES HarnessTest.cpp)
rtype_add_test(NAME engine_tests SOURCES CollisionTest.cpp ComponentQueryTest.cpp ComponentTest.cpp EntityIndexMapTest.cpp EntityRegistryTest.cpp EventBusFixtures.cpp EventBusTest.cpp NetworkEntityTableTest.cpp NetworkIdAllocatorTest.cpp SystemSchedulerTest.cpp TimerSchedulerTest.cpp LIBS rtype_engine GTest::gmock rtype_test_support TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(engine_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles ${RTYPE_TEST_SOURCE_DIR}/engine/Modules ${RTYPE_TEST_SOURCE_DIR}/engine/Commands/Interpreter ${RTYPE_TEST_SOURCE_DIR}/engine/Commands/Settings ${RTYPE_TEST_SOURCE_DIR}/engine/Commands/Configuration)
target_compile_definitions(engine_tests PRIVATE RTYPE_TEST_EXECUTABLE_FOLDER="$<TARGET_FILE_DIR:engine_tests>")
rtype_add_test(NAME game_tests SOURCES WaveEntryPositionTest.cpp CollisionRulesTest.cpp CollisionRulesFixtures.cpp CollisionScene.cpp CollisionSystemTest.cpp DamageRulesTest.cpp DamageRulesFixtures.cpp DamageScene.cpp EntityTypeTest.cpp MovementSystemTest.cpp MovementSystemFixtures.cpp PlayfieldExitSystemFixture.cpp PlayfieldExitSystemTest.cpp ShipStartPositionTest.cpp WorldFixtures.cpp WorldTest.cpp WorldContactPartsTest.cpp WorldContactPartsFixtures.cpp LIBS rtype_game_wave_director rtype_game_wave_progression rtype_game_spawning rtype_game_exceptions rtype_test_shipped_entity_sheets)
target_include_directories(game_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles)
rtype_add_test(NAME network_tests SOURCES NetworkContextTest.cpp UdpSocketFixture.cpp UdpSocketTest.cpp ByteWriterTest.cpp ByteReaderTest.cpp SerializationRoundTripTest.cpp ByteSequence.cpp LIBS rtype_network rtype_network_asio rtype_test_support)
target_include_directories(network_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles ${RTYPE_TEST_SOURCE_DIR}/network/Protocol ${RTYPE_TEST_SOURCE_DIR}/network/Protocol/WorldStateCodec ${RTYPE_TEST_SOURCE_DIR}/network/Compression ${RTYPE_TEST_SOURCE_DIR}/network/Protocol/WorldStateDelta ${RTYPE_TEST_SOURCE_DIR}/network/Reliability ${RTYPE_TEST_SOURCE_DIR}/network/Handshake/Cookies ${RTYPE_TEST_SOURCE_DIR}/network/Handshake/RateLimit ${RTYPE_TEST_SOURCE_DIR}/network/Master/Validators)
rtype_add_test(NAME master_tests LIBS rtype_master_modules rtype_master_launch_options rtype_master_http_module rtype_master_probe_module rtype_network_compression rtype_network_datagram_codec rtype_network_master_probe rtype_master_servers_module rtype_master_servers_helpers rtype_network_master rtype_network_master_client rtype_master_exceptions rtype_master_constants rtype_engine rtype_network_constants rtype_network_exceptions httplib::httplib nlohmann_json::nlohmann_json rtype_test_support TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(master_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/master ${RTYPE_TEST_SOURCE_DIR}/master/Http ${RTYPE_TEST_SOURCE_DIR}/master/Modules ${RTYPE_TEST_SOURCE_DIR}/master/Probe ${RTYPE_TEST_SOURCE_DIR}/master/LaunchOptions ${RTYPE_TEST_SOURCE_DIR}/master/Servers ${RTYPE_TEST_SOURCE_DIR}/master/Servers/Jobs ${RTYPE_TEST_SOURCE_DIR}/master/Servers/Services ${RTYPE_TEST_SOURCE_DIR}/master/Servers/Services/Helpers ${RTYPE_TEST_SOURCE_DIR}/master/Storage ${RTYPE_TEST_SOURCE_DIR}/network/Master/Validators ${RTYPE_TEST_SOURCE_DIR}/network/Master/Client ${RTYPE_TEST_SOURCE_DIR}/doubles ${RTYPE_TEST_SOURCE_DIR}/logging)
rtype_add_test(NAME logging_tests SOURCES LoggerTest.cpp LIBS rtype_logging rtype_test_support)
target_include_directories(logging_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR})
configure_file(${PROJECT_SOURCE_DIR}/assets/waves/waves.toml ${CMAKE_CURRENT_BINARY_DIR}/assets/waves/waves.toml COPYONLY)
file(GLOB RTYPE_SHIPPED_ENTITY_SHEETS CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/assets/entities/*.toml)
foreach(sheet IN LISTS RTYPE_SHIPPED_ENTITY_SHEETS)
  get_filename_component(sheet_file ${sheet} NAME)
  configure_file(${sheet} ${CMAKE_CURRENT_BINARY_DIR}/assets/entities/${sheet_file} COPYONLY)
endforeach()
file(GLOB_RECURSE RTYPE_SHIPPED_SPRITE_SHEETS CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/assets/sprites/*.png)
foreach(sprite_sheet IN LISTS RTYPE_SHIPPED_SPRITE_SHEETS)
  configure_file(${PROJECT_SOURCE_DIR}/assets/${sprite_sheet} ${CMAKE_CURRENT_BINARY_DIR}/assets/${sprite_sheet} COPYONLY)
endforeach()

add_subdirectory(client)
add_subdirectory(concurrency)
add_subdirectory(doubles)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(logging)
add_subdirectory(master)
add_subdirectory(network)
add_subdirectory(server)
add_subdirectory(storage)
add_subdirectory(tools/MalformedDatagrams)
add_subdirectory(tools/LoadTestBots)

# The scripts of tools/network-conditions/ are POSIX shell: their checks run
# where a shell does.
if(UNIX)
  add_test(NAME network_condition_scenarios COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tools/network-conditions/check-scenarios.sh $<TARGET_FILE:r-type_server>)
  add_test(NAME network_condition_commands COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tools/network-conditions/check-printed-commands.sh)
endif()
