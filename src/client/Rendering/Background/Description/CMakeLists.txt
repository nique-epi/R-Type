add_library(rtype_client_background_description STATIC
  BackgroundCatalog.cpp
  BackgroundReading.cpp
)

target_include_directories(rtype_client_background_description PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background_description
  PUBLIC
    SFML::Graphics
    rtype_client_background
    rtype_client_exceptions
    rtype_engine_exceptions
    rtype_engine_time
  PRIVATE
    rtype_client_asset_library
    rtype_engine_data_files
    rtype_logging
)

rtype_enable_warnings(rtype_client_background_description)
