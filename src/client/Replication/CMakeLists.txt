add_library(rtype_client_replication_interfaces INTERFACE)

target_include_directories(rtype_client_replication_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(rtype_client_replication_constants INTERFACE)

target_include_directories(rtype_client_replication_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_replication_constants
  INTERFACE
    rtype_engine_time
    rtype_network_protocol
)

add_library(rtype_client_position_history STATIC PositionHistory.cpp)

target_include_directories(rtype_client_position_history PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_position_history
  PUBLIC
    rtype_client_replication_constants
    rtype_game_components
)

rtype_enable_warnings(rtype_client_position_history)

add_subdirectory(WorldStateFilter)
add_subdirectory(LastListedTicks)
add_subdirectory(EntityReplicator)
add_subdirectory(ScoreBoard)
add_subdirectory(FinalRanking)
