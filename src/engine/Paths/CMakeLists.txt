if(WIN32)
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathWindows.cpp)
elseif(APPLE)
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathMacOs.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathLinux.cpp)
else()
  message(FATAL_ERROR "The engine finds the running executable on Windows, macOS and Linux only")
endif()

add_library(rtype_engine_paths STATIC
  AssetFolder.cpp
  ExecutablePath.cpp
  TextBeforeExtension.cpp
  ${RTYPE_EXECUTABLE_PATH_SOURCE}
)

target_include_directories(rtype_engine_paths PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_paths
  PUBLIC
    rtype_engine_exceptions
  PRIVATE
    rtype_logging
)

target_compile_definitions(rtype_engine_paths
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_engine_paths)
