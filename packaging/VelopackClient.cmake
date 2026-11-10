# Run by cpack once it staged the client, before it makes the client package
# (CPACK_PRE_BUILD_SCRIPTS): packs the staged client with vpk, the tool of
# Velopack, so the client can update itself (src/client/Update/). vpk writes
# into <package directory>/velopack/ the feed releases.<channel>.json, the
# full package R-Type-<version>[-<channel>]-full.nupkg, and what a player
# installs: an installer on Windows (installed for the user, in
# %LocalAppData%\R-Type, with no administrator rights), an AppImage on Linux,
# both moved next to the other packages under the names the download page of
# the web site looks for (R-Type-client-<version>-win64.exe,
# R-Type-client-<version>-Linux.AppImage). On macOS the app vpk signed, which
# holds its updater, replaces the staged one, so the disk image cpack makes
# next is the app the updates know.
#
# vpk is a .NET tool (dotnet tool install -g vpk). Without it the client
# package is made as before, with no update, unless RTYPE_REQUIRE_VELOPACK is
# 1, as the package workflow sets it. RTYPE_RELEASE_NOTES names the Markdown
# notes of the release, which the client shows in WHAT'S NEW once updated;
# RTYPE_MACOS_SIGNING_IDENTITY the identity that signs the app on macOS.
set(VELOPACK_PACKAGE_ID "R-Type")

find_program(VPK vpk)
if(NOT VPK)
  if("$ENV{RTYPE_REQUIRE_VELOPACK}" STREQUAL "1")
    message(FATAL_ERROR "vpk is needed for the client package: dotnet tool install -g vpk")
  endif()
  message(STATUS "No vpk: the client package cannot update itself")
  return()
endif()

set(velopack_output "${CPACK_PACKAGE_DIRECTORY}/velopack")
file(REMOVE_RECURSE "${velopack_output}")
set(velopack_arguments pack
  --packId ${VELOPACK_PACKAGE_ID}
  --packVersion ${CPACK_PACKAGE_VERSION}
  --packTitle "R-Type"
  --packAuthors "${CPACK_PACKAGE_VENDOR}"
  --outputDir "${velopack_output}")
if(NOT "$ENV{RTYPE_RELEASE_NOTES}" STREQUAL "")
  list(APPEND velopack_arguments --releaseNotes "$ENV{RTYPE_RELEASE_NOTES}")
endif()

# The staged client: the folder above bin/r-type_client, or R-Type.app.
file(GLOB_RECURSE staged_files LIST_DIRECTORIES true "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/*")
if(APPLE)
  list(FILTER staged_files INCLUDE REGEX "/R-Type\\.app$")
else()
  list(FILTER staged_files INCLUDE REGEX "/bin/r-type_client(\\.exe)?$")
endif()
list(LENGTH staged_files staged_count)
if(NOT staged_count EQUAL 1)
  message(FATAL_ERROR
    "expected one client under ${CPACK_TEMPORARY_INSTALL_DIRECTORY}, found ${staged_count}")
endif()
list(GET staged_files 0 staged_client)

set(pack_folder "${CPACK_TOPLEVEL_DIRECTORY}/velopack-input")
file(REMOVE_RECURSE "${pack_folder}")

if(WIN32)
  # One folder: the executable, its DLLs and assets/ next to them, where
  # locateAssetFolder() finds them.
  cmake_path(GET staged_client PARENT_PATH binaries)
  cmake_path(GET binaries PARENT_PATH staged)
  file(COPY "${binaries}/" DESTINATION "${pack_folder}")
  file(COPY "${staged}/assets" DESTINATION "${pack_folder}")
  list(APPEND velopack_arguments
    --channel win --packDir "${pack_folder}" --mainExe r-type_client.exe
    --icon "${CMAKE_CURRENT_LIST_DIR}/r-type.ico" --noPortable)
elseif(APPLE)
  list(APPEND velopack_arguments
    --channel osx --packDir "${staged_client}" --mainExe r-type_client
    --icon "${CMAKE_CURRENT_LIST_DIR}/r-type.icns" --noInst)
  set(identity "$ENV{RTYPE_MACOS_SIGNING_IDENTITY}")
  if(NOT identity STREQUAL "")
    list(APPEND velopack_arguments
      --signAppIdentity "${identity}"
      --signEntitlements "${CMAKE_CURRENT_LIST_DIR}/r-type.entitlements")
  endif()
else()
  # The AppDir the AppImage is made of: the executable in usr/bin, its shared
  # libraries in usr/lib, where its rpath points, the assets in usr/assets.
  cmake_path(GET staged_client PARENT_PATH binaries)
  cmake_path(GET binaries PARENT_PATH staged)
  set(pack_folder "${CPACK_TOPLEVEL_DIRECTORY}/R-Type.AppDir")
  file(REMOVE_RECURSE "${pack_folder}")
  file(COPY "${staged}/bin" "${staged}/lib" "${staged}/assets"
    DESTINATION "${pack_folder}/usr")
  file(COPY "${CMAKE_CURRENT_LIST_DIR}/r-type.desktop" "${CMAKE_CURRENT_LIST_DIR}/r-type.png"
    DESTINATION "${pack_folder}")
  file(CREATE_LINK usr/bin/r-type_client "${pack_folder}/AppRun" SYMBOLIC)
  list(APPEND velopack_arguments
    --channel linux --packDir "${pack_folder}" --mainExe usr/bin/r-type_client
    --icon "${CMAKE_CURRENT_LIST_DIR}/r-type.png")
endif()

execute_process(COMMAND "${VPK}" ${velopack_arguments} COMMAND_ERROR_IS_FATAL ANY)

if(WIN32)
  file(RENAME "${velopack_output}/${VELOPACK_PACKAGE_ID}-win-Setup.exe"
    "${CPACK_PACKAGE_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.exe")
elseif(NOT APPLE)
  file(RENAME "${velopack_output}/${VELOPACK_PACKAGE_ID}.AppImage"
    "${CPACK_PACKAGE_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.AppImage")
endif()

if(APPLE)
  # ditto keeps the signature of the app it extracts.
  set(portable "${velopack_output}/${VELOPACK_PACKAGE_ID}-osx-Portable.zip")
  set(extracted "${CPACK_TOPLEVEL_DIRECTORY}/velopack-app")
  file(REMOVE_RECURSE "${extracted}")
  execute_process(COMMAND ditto -x -k "${portable}" "${extracted}" COMMAND_ERROR_IS_FATAL ANY)
  file(GLOB velopack_apps LIST_DIRECTORIES true "${extracted}/*.app")
  list(LENGTH velopack_apps velopack_app_count)
  if(NOT velopack_app_count EQUAL 1)
    message(FATAL_ERROR "expected one app in ${portable}, found ${velopack_app_count}")
  endif()
  list(GET velopack_apps 0 velopack_app)
  file(REMOVE_RECURSE "${staged_client}")
  execute_process(COMMAND ditto "${velopack_app}" "${staged_client}" COMMAND_ERROR_IS_FATAL ANY)
endif()
