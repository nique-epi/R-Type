add_subdirectory(Counting)
add_subdirectory(Recording)

add_library(rtype_network_statistics INTERFACE)

target_link_libraries(rtype_network_statistics
  INTERFACE
    rtype_network_statistics_counting
    rtype_network_statistics_recording
)
