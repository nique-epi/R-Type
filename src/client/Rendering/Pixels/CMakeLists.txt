add_library(rtype_client_pixels INTERFACE)

target_include_directories(rtype_client_pixels INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pixels
  INTERFACE
    SFML::Graphics
    rtype_game
)

add_subdirectory(ShiftedView)
add_subdirectory(RandomRange)
add_subdirectory(PixelRounding)
add_subdirectory(PixelBatch)
add_subdirectory(IntegerScale)
add_subdirectory(EnlargedPicture)
