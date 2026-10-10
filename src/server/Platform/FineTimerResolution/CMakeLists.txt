if(WIN32)
  set(RTYPE_FINE_TIMER_RESOLUTION_SOURCE FineTimerResolutionWindows.cpp)
else()
  set(RTYPE_FINE_TIMER_RESOLUTION_SOURCE FineTimerResolutionPosix.cpp)
endif()

add_library(rtype_server_fine_timer_resolution STATIC ${RTYPE_FINE_TIMER_RESOLUTION_SOURCE})

target_include_directories(rtype_server_fine_timer_resolution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_fine_timer_resolution
  PRIVATE
    rtype_server_constants
    $<$<PLATFORM_ID:Windows>:winmm>
)

target_compile_definitions(rtype_server_fine_timer_resolution
  PRIVATE
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_server_fine_timer_resolution)
