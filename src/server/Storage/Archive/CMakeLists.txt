add_library(rtype_server_storage_archive STATIC ResultsArchive.cpp)

target_include_directories(rtype_server_storage_archive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_storage_archive
  PUBLIC
    rtype_concurrency
    rtype_logging
    rtype_network_leaderboard_request
    rtype_network_transport
    rtype_server_storage_database
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_server_storage_archive)
