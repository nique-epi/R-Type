add_library(rtype_client_sector_map_interfaces INTERFACE)

target_include_directories(rtype_client_sector_map_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sector_map_interfaces
  INTERFACE
    rtype_client_widgets
)

add_subdirectory(SectorLayout)
add_subdirectory(RingedPlanet)
