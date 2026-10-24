add_library(rtype_client_intro_boost_painting STATIC IntroBoostPainting.cpp)

target_include_directories(rtype_client_intro_boost_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_boost_painting
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_cinema_projection
    rtype_client_easing
    rtype_client_intro_hash
    rtype_client_intro_ship_paths
)

rtype_enable_warnings(rtype_client_intro_boost_painting)
