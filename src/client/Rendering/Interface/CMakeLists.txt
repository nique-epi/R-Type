add_library(rtype_client_interface INTERFACE)

target_include_directories(rtype_client_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_interface
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_client_settings_values
    rtype_client_sprites
)

add_subdirectory(TextMeasures)
add_subdirectory(HighContrastColors)
add_subdirectory(TextWriter)
add_subdirectory(InterfacePixel)
add_subdirectory(SmoothImageLayer)
add_subdirectory(InterfaceCanvas)
add_subdirectory(PlayerNumberLabel)
add_subdirectory(PlayerShading)
add_subdirectory(ShipLooks)
add_subdirectory(ShipPicture)
add_subdirectory(BoxCorners)
add_subdirectory(Squadron)
