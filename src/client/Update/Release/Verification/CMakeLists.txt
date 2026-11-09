find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_client_update_release_verification STATIC
  ManifestSignature.cpp
  PackageDigest.cpp
  ReleaseManifestReading.cpp
)

target_include_directories(rtype_client_update_release_verification PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_release_verification
  PUBLIC
    rtype_client_update_release_constants
    rtype_client_update_release_offer
  PRIVATE
    nlohmann_json::nlohmann_json
    OpenSSL::Crypto
    rtype_network_hashing
)

rtype_enable_warnings(rtype_client_update_release_verification)
