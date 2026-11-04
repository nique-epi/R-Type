add_library(rtype_client_asset_library STATIC
  AssetIds.cpp
  AssetLibrary.cpp
)

target_include_directories(rtype_client_asset_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_asset_library
  PUBLIC
    SFML::Graphics
    SFML::System
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_asset_library)
