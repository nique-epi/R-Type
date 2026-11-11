# Run at install time, after every rule of the client on macOS (install
# SCRIPT): signs R-Type.app with the hardened runtime, which notarization
# requires, and the entitlements next to this script. With the Developer ID
# identity that RTYPE_MACOS_SIGNING_IDENTITY names when it is set, timestamped
# by Apple; without an identity otherwise, so macOS reports an unidentified
# developer instead of a damaged app. The libraries of Contents/Frameworks
# (Velopack's) are signed first, with the same identity: a bundle is signed
# only once the code it holds is.
set(identity "$ENV{RTYPE_MACOS_SIGNING_IDENTITY}")
set(timestamp --timestamp)
if(identity STREQUAL "")
  set(identity -)
  set(timestamp "")
endif()
file(GLOB frameworks "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/R-Type.app/Contents/Frameworks/*.dylib")
foreach(framework IN LISTS frameworks)
  execute_process(
    COMMAND codesign --force --options runtime ${timestamp}
      --sign "${identity}" "${framework}"
    COMMAND_ERROR_IS_FATAL ANY)
endforeach()
execute_process(
  COMMAND codesign --force --options runtime ${timestamp}
    --entitlements "${CMAKE_CURRENT_LIST_DIR}/r-type.entitlements"
    --sign "${identity}"
    "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/R-Type.app"
  COMMAND_ERROR_IS_FATAL ANY)
