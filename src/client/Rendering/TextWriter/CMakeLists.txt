add_library(rtype_client_text_writer STATIC TextWriter.cpp)

target_include_directories(rtype_client_text_writer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_text_writer
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_logging
    SFML::Graphics
  PRIVATE
    rtype_client_asset_library
    rtype_client_fragment_shader
)

rtype_enable_warnings(rtype_client_text_writer)
