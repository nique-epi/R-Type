if(WIN32)
  set(RTYPE_ADMIN_CONSOLE_INPUT_SOURCES StandardInputWindows.cpp)
else()
  set(RTYPE_ADMIN_CONSOLE_INPUT_SOURCES StandardInputPosix.cpp)
endif()
add_library(rtype_server_admin_console_input STATIC
  ConsoleLineSplitter.cpp
  ConsoleReader.cpp
  ${RTYPE_ADMIN_CONSOLE_INPUT_SOURCES}
)
target_include_directories(rtype_server_admin_console_input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_server_admin_console_input
  PUBLIC rtype_concurrency_bounded_queue rtype_logging
  PRIVATE rtype_server_admin_console_constants)
target_compile_definitions(rtype_server_admin_console_input PRIVATE $<$<PLATFORM_ID:Windows>:NOMINMAX>)
rtype_enable_warnings(rtype_server_admin_console_input)
