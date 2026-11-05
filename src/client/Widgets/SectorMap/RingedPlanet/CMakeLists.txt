add_library(rtype_client_ringed_planet STATIC RingedPlanet.cpp)

target_include_directories(rtype_client_ringed_planet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ringed_planet
  PUBLIC
    rtype_client_background
    rtype_client_sector_map_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_ringed_planet)
