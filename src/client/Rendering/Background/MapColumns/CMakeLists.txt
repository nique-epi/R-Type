add_library(rtype_client_map_columns STATIC MapColumns.cpp)

target_include_directories(rtype_client_map_columns PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_map_columns
  PUBLIC
    rtype_client_background
)

rtype_enable_warnings(rtype_client_map_columns)
