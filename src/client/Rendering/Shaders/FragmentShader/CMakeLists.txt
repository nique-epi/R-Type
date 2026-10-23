add_library(rtype_client_fragment_shader STATIC FragmentShader.cpp)

target_include_directories(rtype_client_fragment_shader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_fragment_shader
  PUBLIC
    rtype_logging
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_fragment_shader)
