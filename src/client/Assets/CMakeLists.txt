add_library(rtype_client_assets_module STATIC
  AssetIds.cpp
  AssetLibrary.cpp
  AssetLoadingStep.cpp
  ClientAssetsModule.cpp
)

target_include_directories(rtype_client_assets_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_assets_module
  PUBLIC
    SFML::Audio
    SFML::Graphics
    rtype_client_exceptions
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
    rtype_client_module_constants
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_assets_module)
