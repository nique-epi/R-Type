if(WIN32)
  set(RTYPE_STOP_SIGNALS_SOURCE StopSignalsWindows.cpp)
else()
  set(RTYPE_STOP_SIGNALS_SOURCE StopSignalsPosix.cpp)
endif()

add_library(rtype_server_stop_signals STATIC ${RTYPE_STOP_SIGNALS_SOURCE})

target_include_directories(rtype_server_stop_signals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_stop_signals
  PUBLIC
    rtype_server_stop_request
  PRIVATE
    rtype_server_constants
    rtype_server_exceptions
)

target_compile_definitions(rtype_server_stop_signals
  PRIVATE
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_server_stop_signals)
