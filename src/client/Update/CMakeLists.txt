add_subdirectory(Contract)

if(NOT RTYPE_MODULE_UPDATE)
  return()
endif()

# The interfaces and constants the folders below share: IReleaseUpdater,
# ReleaseOffer and UpdateConstants.hpp.
add_library(rtype_client_update_interfaces INTERFACE)

target_include_directories(rtype_client_update_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Velopack)
add_subdirectory(Run)
add_subdirectory(Screens)

add_library(rtype_client_update_module STATIC
  ClientUpdateModule.cpp
  UpdateNotice.cpp
)

target_include_directories(rtype_client_update_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_module
  PUBLIC
    rtype_client_update_contract
    rtype_client_update_run
    rtype_client_update_velopack
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
    rtype_client_exceptions
    rtype_client_platform
    rtype_client_screens_module
    rtype_client_update_screens
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_update_module)
