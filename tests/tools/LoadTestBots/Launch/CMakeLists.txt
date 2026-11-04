add_library(rtype_tools_load_test_bots_launch STATIC BotLaunchOptions.cpp)

target_include_directories(rtype_tools_load_test_bots_launch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_launch
  PUBLIC
    rtype_client_constants
    rtype_network_constants
    rtype_network_protocol
    rtype_server_constants
    rtype_server_launch_options
)

rtype_enable_warnings(rtype_tools_load_test_bots_launch)

rtype_add_test(NAME load_test_bots_launch_tests SOURCES BotLaunchOptionsTest.cpp LIBS rtype_tools_load_test_bots_launch)
