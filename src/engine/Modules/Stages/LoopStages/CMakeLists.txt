add_library(rtype_engine_loop_stages STATIC LoopStages.cpp)

target_include_directories(rtype_engine_loop_stages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_loop_stages
  PUBLIC
    rtype_engine_stages
    rtype_engine_time
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_loop_stages)
