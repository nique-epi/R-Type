add_library(rtype_client_shifted_canvas STATIC ShiftedCanvas.cpp)

target_include_directories(rtype_client_shifted_canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shifted_canvas
  PUBLIC
    rtype_client_interface
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_shifted_canvas)
