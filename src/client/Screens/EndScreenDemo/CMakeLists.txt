add_library(rtype_client_end_screen_demo STATIC EndScreenDemo.cpp)

target_include_directories(rtype_client_end_screen_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_end_screen_demo
  PUBLIC
    rtype_client_replication_interfaces
  PRIVATE
    rtype_client_final_ranking
    rtype_client_screen_interfaces
)

rtype_enable_warnings(rtype_client_end_screen_demo)
