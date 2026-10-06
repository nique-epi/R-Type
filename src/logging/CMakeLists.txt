add_subdirectory(Exceptions)
add_subdirectory(Logger)
add_subdirectory(LogLaunchOptions)

add_library(rtype_logging INTERFACE)
target_include_directories(rtype_logging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_logging
  INTERFACE
    rtype_logging_exceptions
    rtype_logging_logger
    rtype_logging_launch_options
)
