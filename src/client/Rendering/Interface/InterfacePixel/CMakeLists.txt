add_library(rtype_client_interface_pixel STATIC InterfacePixel.cpp)

target_include_directories(rtype_client_interface_pixel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_interface_pixel
  PUBLIC
    SFML::System
  PRIVATE
    rtype_client_rendering_interfaces
)

rtype_enable_warnings(rtype_client_interface_pixel)
