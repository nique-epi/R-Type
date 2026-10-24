add_library(rtype_client_shaders INTERFACE)

target_include_directories(rtype_client_shaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shaders
  INTERFACE
    rtype_client_fragment_shader
    rtype_logging
    SFML::Graphics
)

add_subdirectory(FragmentShader)
