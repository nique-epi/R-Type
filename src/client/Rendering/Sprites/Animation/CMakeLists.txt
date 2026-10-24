add_library(rtype_client_animation STATIC Animation.cpp)

target_include_directories(rtype_client_animation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_animation
  PUBLIC
    rtype_engine_time
    rtype_game_components
)

rtype_enable_warnings(rtype_client_animation)
