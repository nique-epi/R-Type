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

add_subdirectory(src)

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
