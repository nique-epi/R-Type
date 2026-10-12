find_package(Threads REQUIRED)

add_library(rtype_concurrency_bounded_queue INTERFACE)

target_include_directories(rtype_concurrency_bounded_queue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_concurrency_bounded_queue
  INTERFACE
    Threads::Threads
    rtype_concurrency_exceptions
)
