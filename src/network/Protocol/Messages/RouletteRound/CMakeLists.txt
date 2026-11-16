add_library(rtype_network_roulette_round STATIC RouletteRound.cpp)

target_include_directories(rtype_network_roulette_round PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_roulette_round
  PUBLIC
    rtype_network_master_casino_table
    rtype_network_protocol
  PRIVATE
    rtype_network_wire_types
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_roulette_round)
