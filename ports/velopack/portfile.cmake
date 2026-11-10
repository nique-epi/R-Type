# Installs the prebuilt C and C++ library of Velopack from its release
# archive: Velopack is written in Rust, and vcpkg has no port of it. The
# archive is pinned by its SHA-512.
#
# What each system links:
# - Windows: the DLL and its import library, as the other DLLs of the
#   dynamic triplets; the C runtime is the DLL one (/MD), like the project's.
# - macOS: the universal dylib. The static library holds objects built for
#   macOS 26.5, which ld reports on every link; the dylib asks for macOS 11.
#   Its install name is the folder of its own build, so it is renamed
#   @rpath/libvelopack_libc.dylib and signed again, without an identity.
# - Linux: the static library. Its references to symbols newer than glibc
#   2.35 (pidfd_spawnp, statx, ...) are weak, so it links on Ubuntu 22.04.

set(VELOPACK_ARCHIVE_NAME "velopack_libc_${VERSION}.zip")
vcpkg_download_distfile(ARCHIVE
  URLS "https://github.com/velopack/velopack/releases/download/${VERSION}/${VELOPACK_ARCHIVE_NAME}"
  FILENAME "${VELOPACK_ARCHIVE_NAME}"
  SHA512 ad0d9ded09182046cdd113ef2283b07adf633e5a1655f22a1595bc784821fa6a9cd2a22c218fb0e3c54e923d8936dfb08575f8c1c3a08b607ae39a16ec632536
)
vcpkg_extract_source_archive(SOURCE_PATH
  ARCHIVE "${ARCHIVE}"
  NO_REMOVE_ONE_LEVEL
)

file(INSTALL "${SOURCE_PATH}/include/Velopack.h" "${SOURCE_PATH}/include/Velopack.hpp"
  DESTINATION "${CURRENT_PACKAGES_DIR}/include")

if(VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
  set(VELOPACK_ARCHITECTURE x64)
elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
  set(VELOPACK_ARCHITECTURE arm64)
elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "x86")
  set(VELOPACK_ARCHITECTURE x86)
else()
  message(FATAL_ERROR "Velopack ships no library for ${VCPKG_TARGET_ARCHITECTURE}")
endif()

# Every configuration gets the same release binary: the archive has no debug
# one, and vcpkg copies the DLLs of a Debug build from debug/bin.
set(VELOPACK_CONFIGURATION_FOLDERS "${CURRENT_PACKAGES_DIR}" "${CURRENT_PACKAGES_DIR}/debug")

if(VCPKG_TARGET_IS_WINDOWS)
  # The import library names the DLL velopack_libc.dll, whatever the
  # architecture: the DLL is installed under that name, or no program finds it.
  set(VELOPACK_ARCHIVE_LIBRARY "velopack_libc_win_${VELOPACK_ARCHITECTURE}_msvc")
  set(VELOPACK_LIBRARY_NAME "velopack_libc")
  foreach(folder IN LISTS VELOPACK_CONFIGURATION_FOLDERS)
    file(MAKE_DIRECTORY "${folder}/bin" "${folder}/lib")
    file(COPY_FILE "${SOURCE_PATH}/lib/${VELOPACK_ARCHIVE_LIBRARY}.dll"
      "${folder}/bin/${VELOPACK_LIBRARY_NAME}.dll" INPUT_MAY_BE_RECENT)
    file(COPY_FILE "${SOURCE_PATH}/lib/${VELOPACK_ARCHIVE_LIBRARY}.dll.lib"
      "${folder}/lib/${VELOPACK_LIBRARY_NAME}.lib" INPUT_MAY_BE_RECENT)
  endforeach()
  set(VELOPACK_LIBRARY_KIND SHARED)
  set(VELOPACK_LOCATION "bin/${VELOPACK_LIBRARY_NAME}.dll")
  set(VELOPACK_IMPORT_LIBRARY "lib/${VELOPACK_LIBRARY_NAME}.lib")
elseif(VCPKG_TARGET_IS_OSX)
  set(VELOPACK_LIBRARY_NAME "libvelopack_libc.dylib")
  foreach(folder IN LISTS VELOPACK_CONFIGURATION_FOLDERS)
    file(MAKE_DIRECTORY "${folder}/lib")
    file(COPY_FILE "${SOURCE_PATH}/lib/velopack_libc_osx.dylib" "${folder}/lib/${VELOPACK_LIBRARY_NAME}"
      INPUT_MAY_BE_RECENT)
    vcpkg_execute_required_process(
      COMMAND install_name_tool -id "@rpath/${VELOPACK_LIBRARY_NAME}" "${folder}/lib/${VELOPACK_LIBRARY_NAME}"
      WORKING_DIRECTORY "${folder}/lib"
      LOGNAME "install-name-${TARGET_TRIPLET}")
    vcpkg_execute_required_process(
      COMMAND codesign --force --sign - "${folder}/lib/${VELOPACK_LIBRARY_NAME}"
      WORKING_DIRECTORY "${folder}/lib"
      LOGNAME "codesign-${TARGET_TRIPLET}")
  endforeach()
  set(VELOPACK_LIBRARY_KIND SHARED)
  set(VELOPACK_LOCATION "lib/${VELOPACK_LIBRARY_NAME}")
  set(VELOPACK_IMPORT_LIBRARY "")
  set(VCPKG_POLICY_DLLS_IN_STATIC_LIBRARY enabled)
  # vcpkg would rewrite the install name after this file, which voids the
  # signature made above.
  set(VCPKG_FIXUP_MACHO_RPATH OFF)
elseif(VCPKG_TARGET_IS_LINUX)
  set(VELOPACK_LIBRARY_NAME "libvelopack_libc.a")
  foreach(folder IN LISTS VELOPACK_CONFIGURATION_FOLDERS)
    file(MAKE_DIRECTORY "${folder}/lib")
    file(COPY_FILE "${SOURCE_PATH}/lib-static/velopack_libc_linux_${VELOPACK_ARCHITECTURE}_gnu.a"
      "${folder}/lib/${VELOPACK_LIBRARY_NAME}" INPUT_MAY_BE_RECENT)
  endforeach()
  set(VELOPACK_LIBRARY_KIND STATIC)
  set(VELOPACK_LOCATION "lib/${VELOPACK_LIBRARY_NAME}")
  set(VELOPACK_IMPORT_LIBRARY "")
else()
  message(FATAL_ERROR "Velopack ships no library for ${TARGET_TRIPLET}")
endif()

configure_file("${CMAKE_CURRENT_LIST_DIR}/velopack-config.cmake.in"
  "${CURRENT_PACKAGES_DIR}/share/${PORT}/velopack-config.cmake" @ONLY)
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${CMAKE_CURRENT_LIST_DIR}/copyright")

# The binaries come from the archive, as Velopack built them.
set(VCPKG_POLICY_SKIP_ARCHITECTURE_CHECK enabled)
set(VCPKG_POLICY_SKIP_CRT_LINKAGE_CHECK enabled)
set(VCPKG_POLICY_SKIP_MISPLACED_CMAKE_FILES_CHECK enabled)
