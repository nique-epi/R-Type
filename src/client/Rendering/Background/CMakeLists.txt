add_library(rtype_client_background INTERFACE)

target_include_directories(rtype_client_background INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background
  INTERFACE
    SFML::Graphics
    rtype_engine_time
    rtype_game
)

add_subdirectory(RandomRange)
add_subdirectory(PixelRounding)
add_subdirectory(PixelBatch)
add_subdirectory(PlanetImage)
add_subdirectory(SkyEvents)
add_subdirectory(Starfield)
add_subdirectory(GaussianBlur)
add_subdirectory(IntegerScale)
add_subdirectory(ScrollingBackground)
