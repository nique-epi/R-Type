add_library(rtype_client_assets STATIC AssetIds.cpp AssetFolder.cpp)

target_include_directories(rtype_client_assets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_assets
  PUBLIC
    rtype_client_exceptions
    rtype_client_platform
)

rtype_enable_warnings(rtype_client_assets)

add_subdirectory(AssetLibrary)
