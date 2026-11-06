add_library(rtype_network_statistics_recording STATIC
  CsvFile.cpp
  MessageTypeText.cpp
  TrafficCsvRows.cpp
  TrafficLine.cpp
  TrafficRecorder.cpp
)

target_include_directories(rtype_network_statistics_recording PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_statistics_recording
  PUBLIC
    rtype_logging
    rtype_network_statistics_counting
  PRIVATE
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_statistics_recording)
