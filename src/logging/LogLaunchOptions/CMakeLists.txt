add_library(rtype_logging_launch_options STATIC LogLaunchOptions.cpp)

target_include_directories(rtype_logging_launch_options PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_logging_launch_options
  PUBLIC
    rtype_logging_logger
    rtype_logging_exceptions
)

rtype_enable_warnings(rtype_logging_launch_options)
