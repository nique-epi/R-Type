find_package(libhydrogen CONFIG REQUIRED CONFIGS hydrogen-config.cmake)

add_library(rtype_master_server_keys_module STATIC
  ServerKeyRepository.cpp
  ServerKeys.cpp
  ServerKeysModule.cpp
  ServerKeyText.cpp
)

target_include_directories(rtype_master_server_keys_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_server_keys_module
  PUBLIC
    rtype_engine_modules
    rtype_master_storage_module
  PRIVATE
    hydrogen::hydrogen
    rtype_master_constants
)

rtype_enable_warnings(rtype_master_server_keys_module)
