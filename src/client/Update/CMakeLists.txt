add_subdirectory(Contract)

if(NOT RTYPE_MODULE_UPDATE)
  return()
endif()

configure_file(ClientReleaseConstants.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/ClientReleaseConstants.hpp @ONLY)

add_library(rtype_client_update_constants INTERFACE)

target_include_directories(rtype_client_update_constants
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

add_subdirectory(Release)
add_subdirectory(Install)
add_subdirectory(Screens)

add_library(rtype_client_update_module STATIC
  ClientUpdateModule.cpp
  UpdateNotice.cpp
)

target_include_directories(rtype_client_update_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_module
  PUBLIC
    rtype_client_update_contract
    rtype_client_update_install
    rtype_client_update_release
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
    rtype_client_platform
    rtype_client_screens_module
    rtype_client_update_constants
    rtype_client_update_screens
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_update_module)
