if(WIN32)
  set(RTYPE_SERVER_PLATFORM_SOURCES FineTimerResolutionWindows.cpp StopSignalsWindows.cpp ThisMachineWindows.cpp)
elseif(APPLE)
  set(RTYPE_SERVER_PLATFORM_SOURCES FineTimerResolutionPosix.cpp StopSignalsPosix.cpp ThisMachineMacOs.cpp)
else()
  set(RTYPE_SERVER_PLATFORM_SOURCES FineTimerResolutionPosix.cpp StopSignalsPosix.cpp ThisMachineLinux.cpp)
endif()

add_library(rtype_server_platform STATIC ProcCounters.cpp StopRequest.cpp ${RTYPE_SERVER_PLATFORM_SOURCES})

target_include_directories(rtype_server_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_platform
  PUBLIC
    rtype_engine_time
  PRIVATE
    rtype_server_constants
    rtype_server_exceptions
    $<$<PLATFORM_ID:Windows>:winmm>
)

target_compile_definitions(rtype_server_platform
  PRIVATE
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

rtype_enable_warnings(rtype_server_platform)
