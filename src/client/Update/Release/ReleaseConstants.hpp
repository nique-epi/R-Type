#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::client {

/** @brief Where the releases of the game are published: the manifest of the
 * latest one and its signature, then the packages of each release, at
 * RELEASE_DOWNLOAD_PATH, the version, '/' and the file name. */
constexpr std::string_view RELEASES_ORIGIN = "https://github.com";
constexpr std::string_view LATEST_MANIFEST_PATH =
    "/nique-epi/R-Type/releases/latest/download/update-manifest.json";
constexpr std::string_view LATEST_SIGNATURE_PATH =
    "/nique-epi/R-Type/releases/latest/download/update-manifest.sig";
constexpr std::string_view RELEASE_DOWNLOAD_PATH =
    "/nique-epi/R-Type/releases/download/v";
constexpr char RELEASE_PATH_SEPARATOR = '/';

/** @brief How long the check waits to connect, then for each read. */
constexpr std::chrono::seconds RELEASE_CHECK_TIMEOUT{3};

/** @brief The largest manifest read; one holds three packages and at most
 * MAXIMUM_NOTE_LINES notes, about 1 KB. */
constexpr std::size_t MAXIMUM_MANIFEST_BYTES = 16384;
/** @brief The largest package accepted; a client package weighs about
 * 37 MB. */
constexpr std::size_t MAXIMUM_PACKAGE_BYTES = 268435456;
constexpr std::size_t MAXIMUM_PACKAGE_FILE_NAME_SIZE = 128;
/** @brief The notes a manifest may hold, as update-manifest.py bounds them:
 * the lines the panel of WHAT'S NEW holds, each as wide as it. */
constexpr std::size_t MAXIMUM_NOTE_LINES = 8;
constexpr std::size_t MAXIMUM_NOTE_SIZE = 48;
constexpr char FIRST_PRINTABLE_CHARACTER = ' ';
constexpr char LAST_PRINTABLE_CHARACTER = '~';

/** @brief A version is three whole numbers joined by VERSION_SEPARATOR. */
constexpr std::size_t VERSION_PARTS = 3;
constexpr char VERSION_SEPARATOR = '.';

constexpr std::size_t ED25519_PUBLIC_KEY_BYTES = 32;
constexpr std::size_t ED25519_SIGNATURE_BYTES = 64;
constexpr std::size_t SHA256_BYTES = 32;
constexpr std::size_t SHA256_HEXADECIMAL_SIZE = 64;

/** @brief Public half of UPDATE_SIGNING_KEY, the key the release workflow
 * signs every manifest with; printed by `openssl pkey -pubout -outform DER
 * | tail -c 32 | xxd -i` when the key was made. */
constexpr std::array<unsigned char, ED25519_PUBLIC_KEY_BYTES> UPDATE_PUBLIC_KEY{
    0x67, 0xb7, 0x36, 0xaa, 0x81, 0x89, 0x96, 0x7c, 0xe2, 0x9b, 0xca,
    0x41, 0x5c, 0x95, 0x5b, 0x64, 0x42, 0xf4, 0x70, 0x1f, 0x3e, 0xed,
    0x90, 0x61, 0x99, 0x04, 0x32, 0xc4, 0x5b, 0x19, 0x1b, 0x88};

/** @brief Members of the manifest. */
constexpr std::string_view MANIFEST_VERSION_KEY = "version";
constexpr std::string_view MANIFEST_NOTES_KEY = "notes";
constexpr std::string_view MANIFEST_PACKAGES_KEY = "packages";
constexpr std::string_view PACKAGE_FILE_KEY = "file";
constexpr std::string_view PACKAGE_SIZE_KEY = "size";
constexpr std::string_view PACKAGE_SHA256_KEY = "sha256";

}  // namespace rtype::client
