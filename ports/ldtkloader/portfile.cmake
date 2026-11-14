# Builds LDtkLoader from its tagged sources: vcpkg has no port of it. The
# archive is pinned by its SHA-512.
#
# LDtkLoader carries its own copy of nlohmann-json 3.11.3 in its sources. It
# keeps the versioned namespace of nlohmann (json_abi_v3_11_3), so it never
# meets the nlohmann-json of vcpkg (3.12.0) other targets link.
#
# LDTK_NO_THROW stays off: an error throws std::invalid_argument, or an
# exception of nlohmann for a file that is not JSON, instead of ending the
# program.

vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO Madour/LDtkLoader
  REF "${VERSION}"
  SHA512 68efa2d11fe3e271c7061f12c467a4d089c350163cad74d1420a68791941255cfb71cec89296c6964bc939e5c05219c1d142ef8c42c4480fd814de73b908c506
  HEAD_REF master
)

vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    -DLDTK_NO_THROW=OFF
    -DLDTK_BUILD_SFML_EXAMPLE=OFF
    -DLDTK_BUILD_SDL_EXAMPLE=OFF
    -DLDTK_BUILD_RAYLIB_EXAMPLE=OFF
    -DLDTK_BUILD_API_TEST=OFF
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/LDtkLoader)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include"
  "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
