add_library(rtype_client_flight_log_interfaces INTERFACE)

target_include_directories(rtype_client_flight_log_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flight_log_interfaces
  INTERFACE
    rtype_client_effect_interfaces
    rtype_client_replication_interfaces
    rtype_client_sprite_look
    rtype_client_widgets
    rtype_engine_time
)

add_subdirectory(FlightLogTimeline)
add_subdirectory(Taunts)
add_subdirectory(BookMotion)
add_subdirectory(BookPainting)
add_subdirectory(TauntLabel)
add_subdirectory(MissionLogPage)
add_subdirectory(RankingPage)
add_subdirectory(EndChoices)
add_subdirectory(FlightLogPainting)
