add_library(rtype_client_ship_looks STATIC ShipLooks.cpp)

target_include_directories(rtype_client_ship_looks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ship_looks
  PUBLIC
    rtype_client_interface
    rtype_client_settings_values
)

rtype_enable_warnings(rtype_client_ship_looks)
