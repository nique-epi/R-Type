add_library(rtype_logging_throttled_log STATIC ThrottledLog.cpp)

target_include_directories(rtype_logging_throttled_log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_logging_throttled_log
  PUBLIC
    rtype_logging_logger
)

rtype_enable_warnings(rtype_logging_throttled_log)
