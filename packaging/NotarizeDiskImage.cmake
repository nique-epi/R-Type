# Run by cpack after the disk image of the client on macOS
# (CPACK_POST_BUILD_SCRIPTS). With RTYPE_MACOS_SIGNING_IDENTITY set, it signs
# the disk image with that Developer ID identity. With the App Store Connect
# API key too (RTYPE_NOTARY_KEY, the path of the .p8 file, RTYPE_NOTARY_KEY_ID
# and RTYPE_NOTARY_ISSUER), it sends the image to Apple's notary service, waits
# for the verdict, and staples the ticket to it, so Gatekeeper accepts it
# offline. Without them, the image stays as cpack made it.
set(identity "$ENV{RTYPE_MACOS_SIGNING_IDENTITY}")
if(identity STREQUAL "")
  message(STATUS "No RTYPE_MACOS_SIGNING_IDENTITY: the disk image is not signed")
  return()
endif()

set(disk_images ${CPACK_PACKAGE_FILES})
list(FILTER disk_images INCLUDE REGEX "\\.dmg$")
list(LENGTH disk_images disk_image_count)
if(NOT disk_image_count EQUAL 1)
  message(FATAL_ERROR "expected one disk image, cpack made: ${CPACK_PACKAGE_FILES}")
endif()
list(GET disk_images 0 disk_image)

execute_process(
  COMMAND codesign --force --timestamp --sign "${identity}" "${disk_image}"
  COMMAND_ERROR_IS_FATAL ANY)

set(key "$ENV{RTYPE_NOTARY_KEY}")
set(key_id "$ENV{RTYPE_NOTARY_KEY_ID}")
set(issuer "$ENV{RTYPE_NOTARY_ISSUER}")
if(key STREQUAL "" OR key_id STREQUAL "" OR issuer STREQUAL "")
  message(STATUS "No App Store Connect API key: the disk image is signed, not notarized")
  return()
endif()

set(credentials --key "${key}" --key-id "${key_id}" --issuer "${issuer}")
execute_process(
  COMMAND xcrun notarytool submit "${disk_image}" ${credentials}
    --wait --timeout 20m --output-format json
  OUTPUT_VARIABLE submission)
string(JSON status ERROR_VARIABLE unreadable GET "${submission}" status)
if(unreadable)
  message(FATAL_ERROR "the notary service gave no verdict: ${submission}")
endif()
if(NOT status STREQUAL "Accepted")
  string(JSON submission_id GET "${submission}" id)
  execute_process(
    COMMAND xcrun notarytool log "${submission_id}" ${credentials})
  message(FATAL_ERROR "Apple's notary service answered ${status}")
endif()

execute_process(
  COMMAND xcrun stapler staple "${disk_image}"
  COMMAND_ERROR_IS_FATAL ANY)
