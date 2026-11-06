add_library(rtype_server_information_module STATIC
  ServerInformationDesk.cpp
  ServerInformationModule.cpp
)

target_include_directories(rtype_server_information_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_server_information_module PUBLIC RTYPE_GAME_VERSION="${PROJECT_VERSION}")

target_link_libraries(rtype_server_information_module
  PUBLIC
    rtype_engine_application
    rtype_engine_commands
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_logging
    rtype_network_constants
    rtype_network_server_information
    rtype_network_server_information_request
    rtype_network_transport
    rtype_server_network_module
  PRIVATE
    rtype_engine_exceptions
    rtype_engine_time
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_network_statistics_round_trip
    rtype_network_wire_types
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_information_module)
