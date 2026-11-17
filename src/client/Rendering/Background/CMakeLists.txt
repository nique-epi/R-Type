add_library(rtype_client_background INTERFACE)

target_include_directories(rtype_client_background INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background
  INTERFACE
    SFML::Graphics
    rtype_client_pixels
    rtype_engine_time
    rtype_game
)

add_subdirectory(Description)
add_subdirectory(Layers)
add_subdirectory(Composition)
