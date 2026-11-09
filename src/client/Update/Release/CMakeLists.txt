find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_client_update_release STATIC
  ManifestSignature.cpp
  PackageDigest.cpp
  ReleaseCheck.cpp
  ReleaseFetch.cpp
  ReleaseManifestReading.cpp
  ReleaseVersion.cpp
)

target_include_directories(rtype_client_update_release PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_release
  PUBLIC
    rtype_logging
  PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    OpenSSL::Crypto
    rtype_client_update_constants
    rtype_network_hashing
    rtype_network_http_api
)

rtype_enable_warnings(rtype_client_update_release)
