# Run at install time, after every rule of the client on macOS (install
# SCRIPT): signs R-Type.app with the hardened runtime, which notarization
# requires, and the entitlements next to this script. With the Developer ID
# identity that RTYPE_MACOS_SIGNING_IDENTITY names when it is set, timestamped
# by Apple; without an identity otherwise, so macOS reports an unidentified
# developer instead of a damaged app. The bundle holds one executable and no
# other code, so signing the bundle signs all of it.
set(identity "$ENV{RTYPE_MACOS_SIGNING_IDENTITY}")
set(timestamp --timestamp)
if(identity STREQUAL "")
  set(identity -)
  set(timestamp "")
endif()
execute_process(
  COMMAND codesign --force --options runtime ${timestamp}
    --entitlements "${CMAKE_CURRENT_LIST_DIR}/r-type.entitlements"
    --sign "${identity}"
    "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/R-Type.app"
  COMMAND_ERROR_IS_FATAL ANY)
