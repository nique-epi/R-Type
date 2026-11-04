add_library(rtype_server_storage_database STATIC ResultsDatabase.cpp)

target_include_directories(rtype_server_storage_database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_storage_database
  PUBLIC
    rtype_network_leaderboard_answer
    rtype_network_leaderboard_request
    rtype_storage
  PRIVATE
    rtype_engine_paths
    rtype_network_protocol
)

rtype_enable_warnings(rtype_server_storage_database)
