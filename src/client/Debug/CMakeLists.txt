if(NOT RTYPE_MODULE_DEBUG)
  return()
endif()

add_subdirectory(Panel)
add_subdirectory(Journal)

add_library(rtype_client_debug_module STATIC ClientDebugModule.cpp)

target_include_directories(rtype_client_debug_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_debug_module
  PUBLIC
    rtype_client_debug_journal
    rtype_client_debug_panel
  PRIVATE
    rtype_client_constants
)

rtype_enable_warnings(rtype_client_debug_module)
