cmake_minimum_required(VERSION 4.2)

project(r-type VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" OFF)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

# The sources are UTF-8, and the text shown on screen is decoded as UTF-8
# (sf::String::fromUtf8). GCC and Clang read and store UTF-8 by default; MSVC
# reads a source without a byte order mark in the current code page and
# stores its strings in it, unless /utf-8 sets both to UTF-8.
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

function(rtype_enable_warnings target)
  target_compile_options(${target} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
  )
endfunction()

# Declares a plugin: a MODULE library named <NAME>.so or <NAME>.dll, in the
# plugins/ folder next to the programs. On Windows it exports rtypePluginEntry
# only; on Linux and macOS it keeps the default visibility, and the program
# looks up rtypePluginEntry alone. On Windows the library is a runtime artifact,
# so both output directories are set.
# WITHOUT_ENTRY is for a test library that defines no rtypePluginEntry: the
# Windows export list would name a symbol it does not have.
function(rtype_add_plugin)
  cmake_parse_arguments(ARG "WITHOUT_ENTRY" "TARGET;NAME" "SOURCES;LIBS" ${ARGN})
  add_library(${ARG_TARGET} MODULE ${ARG_SOURCES})
  target_link_libraries(${ARG_TARGET} PRIVATE rtype_engine_modules ${ARG_LIBS})
  set_target_properties(${ARG_TARGET} PROPERTIES
    PREFIX ""
    OUTPUT_NAME ${ARG_NAME}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins)
  if(WIN32 AND NOT ARG_WITHOUT_ENTRY)
    target_sources(${ARG_TARGET} PRIVATE ${RTYPE_PLUGIN_ENTRY_DEFINITION})
  endif()
  rtype_enable_warnings(${ARG_TARGET})
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

option(ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

# The sanitizers instrument every target of the repository. LeakSanitizer
# runs with AddressSanitizer by default on Linux x86_64 only. MSVC is left
# out: its AddressSanitizer cannot see into the vcpkg libraries, built
# without it.
if(ENABLE_SANITIZERS)
  if(MSVC)
    message(FATAL_ERROR "ENABLE_SANITIZERS needs GCC or Clang")
  endif()
  add_compile_options(-fsanitize=address,undefined
    -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# A plugin is a MODULE library that links the static libraries of the engine it
# uses: on Linux, a static library linked into a shared one must be
# position-independent.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)

# The archives and the AppImage hold bin/ and assets/ side by side, the macOS
# app holds MacOS/ and Resources/assets/: locateAssetFolder() finds assets/ in
# the folder above the executable or in Resources/ beside it, whatever folder
# it is launched from.
#
# Each rule belongs to the package of one program: the client one holds the
# assets it draws and plays, the server one the waves it spawns.
include(GNUInstallDirs)

# On macOS the client is the R-Type.app bundle: its executable in
# Contents/MacOS, its assets in Contents/Resources, where Apple wants every file
# that is not code, so the signature seals them.
if(APPLE)
  set(RTYPE_CLIENT_BINARY_FOLDER R-Type.app/Contents/MacOS)
  set(RTYPE_CLIENT_ASSETS_PARENT R-Type.app/Contents/Resources)
  configure_file(packaging/Info.plist.in Info.plist @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/Info.plist
    DESTINATION R-Type.app/Contents COMPONENT client
  )
  install(FILES packaging/r-type.icns
    DESTINATION ${RTYPE_CLIENT_ASSETS_PARENT} COMPONENT client
  )
else()
  set(RTYPE_CLIENT_BINARY_FOLDER ${CMAKE_INSTALL_BINDIR})
  set(RTYPE_CLIENT_ASSETS_PARENT .)
endif()

# On Windows the executable of the client carries the icon of the game.
if(WIN32)
  configure_file(packaging/r-type.rc.in r-type.rc @ONLY)
  target_sources(r-type_client PRIVATE ${CMAKE_BINARY_DIR}/r-type.rc)
endif()

# The shared libraries each program needs are collected on Windows and Linux;
# on macOS the app needs none but the system's.
if(APPLE)
  install(TARGETS r-type_client
    RUNTIME DESTINATION ${RTYPE_CLIENT_BINARY_FOLDER} COMPONENT client
  )
  install(TARGETS r-type_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT server
  )
else()
  install(TARGETS r-type_client
    RUNTIME_DEPENDENCY_SET rtype_client_libraries
    RUNTIME DESTINATION ${RTYPE_CLIENT_BINARY_FOLDER} COMPONENT client
  )
  install(TARGETS r-type_server
    RUNTIME_DEPENDENCY_SET rtype_server_libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT server
  )
endif()
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
  DESTINATION ${RTYPE_CLIENT_ASSETS_PARENT} COMPONENT client
  PATTERN waves EXCLUDE
  PATTERN compression EXCLUDE
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/waves ${CMAKE_SOURCE_DIR}/assets/entities
  DESTINATION assets COMPONENT server
)

# Once the bundle is complete, SignApp.cmake signs it with the hardened
# runtime. This rule stays after every rule of the client.
if(APPLE)
  install(SCRIPT packaging/SignApp.cmake COMPONENT client)
endif()

# vcpkg links its libraries statically on Linux and macOS and as DLLs on
# Windows. The DLLs are found in the vcpkg folder; the system ones and the
# API sets are left to Windows. The MSVC runtime goes in both packages, so
# InstallRequiredSystemLibraries only lists it.
if(WIN32)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include(InstallRequiredSystemLibraries)
  foreach(program IN ITEMS client server)
    install(RUNTIME_DEPENDENCY_SET rtype_${program}_libraries
      DIRECTORIES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin
      PRE_EXCLUDE_REGEXES "^api-ms-" "^ext-ms-"
      POST_EXCLUDE_REGEXES ".*[Ss]ystem32/.*"
      DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${program}
    )
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${program}
    )
  endforeach()
elseif(NOT APPLE)
  # On Linux the package carries the shared libraries the programs need (the
  # X11 extensions, udev), next to bin/ in lib/, except those every system
  # has and must provide itself: the C library, the C++ runtime, the graphics
  # drivers. The list is the excludelist of the AppImage project.
  set(RTYPE_LIBRARIES_OF_EVERY_LINUX
    "^ld-linux" "^linux-vdso" "^libc\\." "^libdl\\." "^libm\\." "^libmvec\\."
    "^libpthread\\." "^librt\\." "^libresolv\\." "^libutil\\." "^libanl\\."
    "^libnss_" "^libthread_db\\." "^libBrokenLocale\\." "^libcidn\\."
    "^libstdc\\+\\+\\." "^libgcc_s\\."
    "^libGL\\." "^libEGL\\." "^libGLdispatch\\." "^libGLX\\." "^libOpenGL\\."
    "^libdrm\\." "^libglapi\\." "^libgbm\\."
    "^libxcb\\." "^libxcb-dri[23]\\." "^libX11\\." "^libX11-xcb\\."
    "^libwayland-client\\." "^libasound\\." "^libjack\\." "^libpipewire-"
    "^libfontconfig\\." "^libfreetype\\." "^libharfbuzz\\." "^libfribidi\\."
    "^libexpat\\." "^libz\\." "^libICE\\." "^libSM\\." "^libusb-1\\.0\\."
    "^libuuid\\." "^libcom_err\\." "^libgpg-error\\." "^libgmp\\."
  )
  foreach(program IN ITEMS client server)
    install(RUNTIME_DEPENDENCY_SET rtype_${program}_libraries
      PRE_EXCLUDE_REGEXES ${RTYPE_LIBRARIES_OF_EVERY_LINUX}
      DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${program}
    )
  endforeach()
  set_target_properties(r-type_client r-type_server PROPERTIES
    INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
  )
endif()

set(CPACK_PACKAGE_NAME "R-Type")
set(CPACK_PACKAGE_VENDOR "R-Type team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Networked shoot'em up")
# The project has no licence file: without this, the installer would show
# CPack's placeholder licence.
set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
# The icon of the game on the installer and the uninstaller; the executable
# carries it through r-type.rc.
set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/packaging/r-type.ico)
set(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/packaging/r-type.ico)
# The packages hold no debug symbols.
set(CPACK_STRIP_FILES ON)
# One archive per component; each package config lists a single one.
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DMG_VOLUME_NAME "R-Type")
set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/packaging/CPackGeneratorConfig.cmake)
include(CPack)

# One CPack config per program, read by the package presets "client" and
# "server" instead of CPackConfig.cmake. The server gets a plain archive, and
# an installer on Windows; the client gets the formats a player opens: an app
# in a disk image on macOS, signed and notarized when the credentials are
# given, and an AppImage next to its archive on Linux.
foreach(RTYPE_PACKAGE_PART IN ITEMS client server)
  string(TOUPPER ${RTYPE_PACKAGE_PART} RTYPE_PACKAGE_PART_UPPER)
  set(RTYPE_PACKAGE_TITLE "R-Type Server")
  set(RTYPE_POST_BUILD_SCRIPTS "")
  if(WIN32)
    set(RTYPE_PACKAGE_GENERATORS "ZIP;NSIS")
  else()
    set(RTYPE_PACKAGE_GENERATORS "TGZ")
  endif()
  if(RTYPE_PACKAGE_PART STREQUAL "client")
    set(RTYPE_PACKAGE_TITLE "R-Type")
    if(APPLE)
      set(RTYPE_PACKAGE_GENERATORS "DragNDrop")
      set(RTYPE_POST_BUILD_SCRIPTS ${CMAKE_SOURCE_DIR}/packaging/NotarizeDiskImage.cmake)
    elseif(NOT WIN32)
      set(RTYPE_POST_BUILD_SCRIPTS ${CMAKE_SOURCE_DIR}/packaging/AppImage.cmake)
    endif()
  endif()
  configure_file(packaging/CPackPartConfig.cmake.in
    CPackConfig-${RTYPE_PACKAGE_PART}.cmake @ONLY)
endforeach()

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

# Fails the configure when a target reaches a forbidden library through any of its links, PRIVATE ones included.
function(rtype_forbid_reachable_links target)
  set(pending ${target})
  set(reached "")
  while(pending)
    list(POP_FRONT pending current)
    if(current IN_LIST reached OR NOT TARGET ${current})
      continue()
    endif()
    list(APPEND reached ${current})
    get_target_property(linked ${current} LINK_LIBRARIES)
    get_target_property(interface_linked ${current} INTERFACE_LINK_LIBRARIES)
    foreach(library IN LISTS linked interface_linked)
      string(REGEX REPLACE "^\\$<LINK_ONLY:(.+)>$" "\\1" library "${library}")
      foreach(forbidden IN LISTS ARGN)
        if(library MATCHES "^${forbidden}")
          message(FATAL_ERROR "${target} must not reach ${library}, linked by ${current}")
        endif()
      endforeach()
      list(APPEND pending ${library})
    endforeach()
  endwhile()
endfunction()

rtype_forbid_links(rtype_engine "SFML::" "asio::" "rtype_game" "rtype_network" "rtype_client")
rtype_forbid_links(rtype_game "SFML::" "asio::" "rtype_network" "rtype_client")
rtype_forbid_links(rtype_network "SFML::" "rtype_game" "rtype_engine" "rtype_client")
rtype_forbid_links(rtype_logging "SFML::" "asio::" "rtype_engine" "rtype_game" "rtype_network" "rtype_client" "rtype_concurrency" "rtype_identifiers")
rtype_forbid_links(rtype_concurrency "SFML::" "asio::" "rtype_engine" "rtype_game" "rtype_network" "rtype_client" "rtype_identifiers")
rtype_forbid_links(rtype_identifiers "SFML::" "asio::" "rtype_engine" "rtype_game" "rtype_network" "rtype_client" "rtype_logging" "rtype_concurrency")
rtype_forbid_links(rtype_storage "SFML::" "asio::" "rtype_engine" "rtype_game" "rtype_network" "rtype_client" "rtype_logging" "rtype_concurrency" "rtype_identifiers")
rtype_forbid_links(r-type_server "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_network_module "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_game_statistics_module "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_network_datagram_relay "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_network_compression "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_platform "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_launch_options "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_replication_module "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_replication_announcements_milestones "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_replication_announcements_streams "SFML::" "asio::" "rtype_network_asio" "rtype_client")
rtype_forbid_links(rtype_server_replication_wire "SFML::" "asio::" "rtype_network_asio" "rtype_client")
set(RTYPE_MASTER_FORBIDDEN_LINKS "SFML::" "asio::" "rtype_network_asio" "rtype_game" "rtype_client" "rtype_server")
rtype_forbid_reachable_links(rtype_master_modules ${RTYPE_MASTER_FORBIDDEN_LINKS})
rtype_forbid_reachable_links(rtype_master_launch_options ${RTYPE_MASTER_FORBIDDEN_LINKS})

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
