add_subdirectory(Exceptions)
add_subdirectory(BoundedQueue)
add_subdirectory(InterruptibleSleep)
add_subdirectory(LatestValue)

add_library(rtype_concurrency INTERFACE)

target_link_libraries(rtype_concurrency
  INTERFACE
    rtype_concurrency_exceptions
    rtype_concurrency_bounded_queue
    rtype_concurrency_interruptible_sleep
    rtype_concurrency_latest_value
)
