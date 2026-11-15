add_library(rtype_client_reporting_texture_source STATIC ReportingTextureSource.cpp)

target_include_directories(rtype_client_reporting_texture_source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_reporting_texture_source
  PUBLIC
    rtype_client_sprites
    rtype_logging
)

rtype_enable_warnings(rtype_client_reporting_texture_source)
