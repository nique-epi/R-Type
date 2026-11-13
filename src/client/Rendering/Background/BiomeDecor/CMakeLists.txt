add_library(rtype_client_biome_decor STATIC BiomeDecor.cpp)

target_include_directories(rtype_client_biome_decor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_biome_decor
  PUBLIC
    rtype_client_background
)

rtype_enable_warnings(rtype_client_biome_decor)
