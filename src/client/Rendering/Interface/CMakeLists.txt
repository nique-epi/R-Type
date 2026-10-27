add_library(rtype_client_interface INTERFACE)

target_include_directories(rtype_client_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_interface
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_client_sprites
)

add_subdirectory(TextWriter)
add_subdirectory(InterfacePixel)
add_subdirectory(InterfaceCanvas)
add_subdirectory(PlayerNumberLabel)
add_subdirectory(Squadron)
