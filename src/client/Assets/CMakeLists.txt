add_library(rtype_client_assets STATIC AssetIds.cpp)

target_include_directories(rtype_client_assets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

rtype_enable_warnings(rtype_client_assets)

add_subdirectory(AssetLibrary)
add_subdirectory(AssetLoadingStep)
