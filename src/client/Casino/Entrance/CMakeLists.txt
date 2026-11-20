add_subdirectory(Lights)
add_subdirectory(Doors)

add_library(rtype_client_casino_entrance INTERFACE)

target_link_libraries(rtype_client_casino_entrance
  INTERFACE
    rtype_client_casino_entrance_lights
    rtype_client_casino_entrance_doors
)
