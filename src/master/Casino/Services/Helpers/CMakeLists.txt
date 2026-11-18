add_library(rtype_master_casino_helpers STATIC
  CasinoRoomNames.cpp
  RoundPayouts.cpp
  RoundReference.cpp
  RouletteSchedule.cpp
)

target_include_directories(rtype_master_casino_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_casino_helpers
  PUBLIC
    rtype_master_casino_models
  PRIVATE
    rtype_network_master_casino_cloth
)

rtype_enable_warnings(rtype_master_casino_helpers)
