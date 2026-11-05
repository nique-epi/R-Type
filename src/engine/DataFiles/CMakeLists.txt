find_package(tomlplusplus CONFIG REQUIRED)

add_library(rtype_engine_data_files STATIC DataFileReading.cpp)

target_include_directories(rtype_engine_data_files PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_data_files
  PUBLIC
    rtype_engine_exceptions
    rtype_engine_time
    tomlplusplus::tomlplusplus
)

rtype_enable_warnings(rtype_engine_data_files)
