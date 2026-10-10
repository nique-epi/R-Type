cmake_minimum_required(VERSION 3.28)

project(r-type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" OFF)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

function(rtype_enable_warnings target)
  target_compile_options(${target} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
  )
endfunction()

# LOG_LEVEL is the lowest log level compiled into the binaries. It is
# translated into RT_LOG_BUILD_LEVEL (0..5) and injected as a compile
# definition: Logger.hpp wraps each logging method in `if constexpr`, so calls
# below this level produce no code at all.
#
# The default keeps every level in every build type, so that the level chosen
# when a program is launched always takes effect.
if(NOT DEFINED LOG_LEVEL)
  set(LOG_LEVEL "trace")
endif()

string(TOLOWER "${LOG_LEVEL}" LOG_LEVEL_LOWER)
if(LOG_LEVEL_LOWER STREQUAL "trace")
  set(RT_LOG_BUILD_LEVEL 0)
elseif(LOG_LEVEL_LOWER STREQUAL "debug")
  set(RT_LOG_BUILD_LEVEL 1)
elseif(LOG_LEVEL_LOWER STREQUAL "info")
  set(RT_LOG_BUILD_LEVEL 2)
elseif(LOG_LEVEL_LOWER STREQUAL "warn" OR LOG_LEVEL_LOWER STREQUAL "warning")
  set(RT_LOG_BUILD_LEVEL 3)
elseif(LOG_LEVEL_LOWER STREQUAL "error")
  set(RT_LOG_BUILD_LEVEL 4)
elseif(LOG_LEVEL_LOWER STREQUAL "silent"
       OR LOG_LEVEL_LOWER STREQUAL "off"
       OR LOG_LEVEL_LOWER STREQUAL "none")
  set(RT_LOG_BUILD_LEVEL 5)
else()
  message(FATAL_ERROR
    "Invalid LOG_LEVEL='${LOG_LEVEL}'. "
    "Expected one of: trace, debug, info, warn, error, silent.")
endif()

message(STATUS
  "Log level: ${LOG_LEVEL_LOWER} (RT_LOG_BUILD_LEVEL=${RT_LOG_BUILD_LEVEL})")
add_compile_definitions(RT_LOG_BUILD_LEVEL=${RT_LOG_BUILD_LEVEL})

add_subdirectory(src)

function(rtype_forbid_links target)
  get_target_property(linked ${target} LINK_LIBRARIES)
  get_target_property(interface_linked ${target} INTERFACE_LINK_LIBRARIES)
  foreach(forbidden IN LISTS ARGN)
    foreach(library IN LISTS linked interface_linked)
      if(library MATCHES "^${forbidden}")
        message(FATAL_ERROR "${target} must not link ${library}")
      endif()
    endforeach()
  endforeach()
endfunction()

rtype_forbid_links(rtype_engine "SFML::" "asio::" "rtype_game" "rtype_network" "rtype_client")
rtype_forbid_links(rtype_game "SFML::" "asio::" "rtype_network" "rtype_client")
rtype_forbid_links(rtype_network "SFML::" "rtype_game" "rtype_engine" "rtype_client")
rtype_forbid_links(rtype_logging "SFML::" "asio::" "rtype_engine" "rtype_game" "rtype_network" "rtype_client")
rtype_forbid_links(r-type_server "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_application "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_tick_loop "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_datagram_relay "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_fine_timer_resolution "SFML::" "asio::" "rtype_network_asio" "rtype_client")

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

file(GLOB_RECURSE RTYPE_CPP_FILES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/tests/*.cpp
)
file(GLOB_RECURSE RTYPE_HPP_FILES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.hpp
  ${CMAKE_SOURCE_DIR}/tests/*.hpp
)

find_program(CLANG_FORMAT NAMES clang-format-20 clang-format)
if(CLANG_FORMAT)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${RTYPE_CPP_FILES} ${RTYPE_HPP_FILES}
    COMMENT "Formatting code with ${CLANG_FORMAT}..."
  )
  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run -Werror ${RTYPE_CPP_FILES} ${RTYPE_HPP_FILES}
    COMMENT "Checking code format with ${CLANG_FORMAT}..."
  )
endif()

find_program(CLANG_TIDY NAMES clang-tidy-20 clang-tidy)
if(CLANG_TIDY)
  add_custom_target(tidy
    COMMAND ${CLANG_TIDY} -p ${CMAKE_BINARY_DIR} ${RTYPE_CPP_FILES}
    COMMENT "Linting code with ${CLANG_TIDY}..."
  )
endif()
