add_library(rtype_server_storage STATIC
  ResultsArchive.cpp
  ResultsDatabase.cpp
)

target_include_directories(rtype_server_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_storage
  PUBLIC
    rtype_concurrency
    rtype_logging
    rtype_network_leaderboard_answer
    rtype_network_leaderboard_request
    rtype_network_transport
    rtype_storage
  PRIVATE
    rtype_engine_paths
)

rtype_enable_warnings(rtype_server_storage)
