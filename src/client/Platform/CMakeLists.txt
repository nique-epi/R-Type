if(WIN32)
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathWindows.cpp)
elseif(APPLE)
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathMacOs.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(RTYPE_EXECUTABLE_PATH_SOURCE ExecutablePathLinux.cpp)
else()
  message(FATAL_ERROR "The client finds its executable on Windows, macOS and Linux only")
endif()

add_library(rtype_client_platform STATIC ${RTYPE_EXECUTABLE_PATH_SOURCE})

target_include_directories(rtype_client_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_client_platform
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_client_platform)
