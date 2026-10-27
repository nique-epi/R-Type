add_library(rtype_client_final_ranking STATIC FinalRanking.cpp)

target_include_directories(rtype_client_final_ranking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_final_ranking
  PUBLIC
    rtype_client_replication_interfaces
    rtype_network_game_ended
)

rtype_enable_warnings(rtype_client_final_ranking)
