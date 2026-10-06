find_package(Threads REQUIRED)

add_library(rtype_logging_logger STATIC Logger.cpp)

target_include_directories(rtype_logging_logger
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(rtype_logging_logger
  PUBLIC
    Threads::Threads
    rtype_logging_exceptions
)

rtype_enable_warnings(rtype_logging_logger)
