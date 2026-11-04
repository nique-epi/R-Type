add_library(rtype_client_asset_loading STATIC
  AssetLoadingStep.cpp
  AssetProblemRecording.cpp
)

target_include_directories(rtype_client_asset_loading PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_asset_loading
  PUBLIC
    rtype_client_asset_library
    rtype_client_exceptions
  PRIVATE
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_asset_loading)
