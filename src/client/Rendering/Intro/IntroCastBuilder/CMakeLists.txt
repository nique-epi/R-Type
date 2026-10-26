add_library(rtype_client_intro_cast_builder STATIC IntroCastBuilder.cpp)

target_include_directories(rtype_client_intro_cast_builder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_cast_builder
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_intro_mob_paths
    rtype_client_intro_ship_paths
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_intro_cast_builder)
