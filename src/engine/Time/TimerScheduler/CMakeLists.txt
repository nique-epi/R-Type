add_library(rtype_engine_timer_scheduler STATIC TimerScheduler.cpp)

target_include_directories(rtype_engine_timer_scheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_timer_scheduler
  PUBLIC
    rtype_engine_time
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_timer_scheduler)
