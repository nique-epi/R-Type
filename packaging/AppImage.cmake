# Run by cpack after the client archive on Linux (CPACK_POST_BUILD_SCRIPTS):
# turns the staged client into an AppImage next to the archive. The executable
# goes to usr/bin, its shared libraries to usr/lib, where its rpath points, and
# the assets to usr/assets, the folder above it, where locateAssetFolder()
# finds them.
find_program(APPIMAGETOOL appimagetool)
if(NOT APPIMAGETOOL)
  message(FATAL_ERROR "appimagetool is needed for the AppImage of the client")
endif()
file(GLOB_RECURSE clients "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/*")
list(FILTER clients INCLUDE REGEX "/bin/r-type_client$")
list(LENGTH clients client_count)
if(NOT client_count EQUAL 1)
  message(FATAL_ERROR
    "expected one r-type_client under ${CPACK_TEMPORARY_INSTALL_DIRECTORY}, "
    "found ${client_count}")
endif()
list(GET clients 0 client)
cmake_path(GET client PARENT_PATH binaries)
cmake_path(GET binaries PARENT_PATH staged)

set(app_folder "${CPACK_TOPLEVEL_DIRECTORY}/R-Type.AppDir")
file(REMOVE_RECURSE "${app_folder}")
file(COPY "${staged}/bin" "${staged}/lib" "${staged}/assets"
  DESTINATION "${app_folder}/usr")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/r-type.desktop"
  "${CMAKE_CURRENT_LIST_DIR}/r-type.png" DESTINATION "${app_folder}")
file(CREATE_LINK usr/bin/r-type_client "${app_folder}/AppRun" SYMBOLIC)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E env ARCH=x86_64
    ${APPIMAGETOOL} --no-appstream "${app_folder}"
    "${CPACK_PACKAGE_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.AppImage"
  COMMAND_ERROR_IS_FATAL ANY)
