find_package(tomlplusplus CONFIG REQUIRED)

add_library(rtype_server_wave_catalog STATIC WaveCatalog.cpp)

target_include_directories(rtype_server_wave_catalog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_wave_catalog
  PUBLIC
    rtype_server_wave_plan
  PRIVATE
    rtype_server_exceptions
    tomlplusplus::tomlplusplus
)

rtype_enable_warnings(rtype_server_wave_catalog)
