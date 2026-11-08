find_package(boost_process CONFIG REQUIRED)

if(WIN32)
  set(RTYPE_CHILD_BINDING_SOURCE ChildBindingWindows.cpp)
else()
  set(RTYPE_CHILD_BINDING_SOURCE ChildBindingPosix.cpp)
endif()

add_library(rtype_server_processes STATIC
  BoostChildProcess.cpp
  BoostProcessLauncher.cpp
  ChildOutputLines.cpp
  ChildOutputRelay.cpp
  ${RTYPE_CHILD_BINDING_SOURCE}
)

target_include_directories(rtype_server_processes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_server_processes
  PUBLIC
    BOOST_PROCESS_USE_STD_FS
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
  PRIVATE
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

target_link_libraries(rtype_server_processes
  PUBLIC
    Boost::process
  PRIVATE
    rtype_logging
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_processes)
