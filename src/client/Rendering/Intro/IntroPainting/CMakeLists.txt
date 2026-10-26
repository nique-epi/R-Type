add_library(rtype_client_intro_painting STATIC IntroPainting.cpp)

target_include_directories(rtype_client_intro_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_painting
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_intro_boost_painting
    rtype_client_intro_fire_painting
    rtype_client_intro_fleet_painting
)

rtype_enable_warnings(rtype_client_intro_painting)
