add_library(rtype_client_kill_chain STATIC KillChain.cpp)

target_include_directories(rtype_client_kill_chain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_kill_chain
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_kill_chain)
