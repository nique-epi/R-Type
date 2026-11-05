add_subdirectory(Library)
add_subdirectory(Loading)

add_library(rtype_client_assets_module STATIC
  ClientAssetsModule.cpp
)

target_include_directories(rtype_client_assets_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_assets_module
  PUBLIC
    rtype_client_asset_library
    rtype_client_asset_loading
    rtype_client_entity_appearances
    rtype_engine_install_context
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_assets_module)
