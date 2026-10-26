add_library(rtype_client_intro_ship_paths STATIC IntroShipPaths.cpp)

target_include_directories(rtype_client_intro_ship_paths PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_ship_paths
  PUBLIC
    rtype_client_intro_timeline
  PRIVATE
    rtype_client_easing
)

rtype_enable_warnings(rtype_client_intro_ship_paths)
