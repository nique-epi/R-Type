#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/** @brief Where the releases of the game are published: the feeds of the
 * latest one at LATEST_RELEASE_PATH and the feed name, each with its
 * signature at the same name followed by SIGNATURE_SUFFIX; the packages of a
 * release at RELEASE_DOWNLOAD_PATH, its version, '/' and the file name. */
constexpr std::string_view RELEASES_ORIGIN = "https://github.com";
constexpr std::string_view LATEST_RELEASE_PATH =
    "/nique-epi/R-Type/releases/latest/download/";
constexpr std::string_view RELEASE_DOWNLOAD_PATH =
    "/nique-epi/R-Type/releases/download/v";
constexpr char RELEASE_PATH_SEPARATOR = '/';
constexpr std::string_view SIGNATURE_SUFFIX = ".sig";

/** @brief How long a request waits to connect, then for each read. */
constexpr std::chrono::seconds RELEASE_REQUEST_TIMEOUT{5};

/** @brief The largest feed read: one release of one system is about 1 KB, so
 * this leaves room for many releases. */
constexpr std::size_t MAXIMUM_FEED_BYTES = 262144;
/** @brief The largest package downloaded; a client package weighs about
 * 40 MB. */
constexpr std::uint64_t MAXIMUM_PACKAGE_BYTES = 536870912;
/** @brief The longest name of a feed or of a package, which holds only
 * letters, digits and FILE_NAME_PUNCTUATION and never starts with a dot. */
constexpr std::size_t MAXIMUM_FILE_NAME_SIZE = 128;
constexpr std::string_view FILE_NAME_PUNCTUATION = ".-_";
constexpr char FILE_NAME_DOT = '.';

constexpr std::size_t ED25519_PUBLIC_KEY_BYTES = 32;
constexpr std::size_t ED25519_SIGNATURE_BYTES = 64;

/** @brief Public half of UPDATE_SIGNING_KEY, the key the release workflow
 * signs every feed with; printed by `openssl pkey -pubout -outform DER
 * | tail -c 32 | xxd -i` when the key was made. */
constexpr std::array<unsigned char, ED25519_PUBLIC_KEY_BYTES> UPDATE_PUBLIC_KEY{
    0x67, 0xb7, 0x36, 0xaa, 0x81, 0x89, 0x96, 0x7c, 0xe2, 0x9b, 0xca,
    0x41, 0x5c, 0x95, 0x5b, 0x64, 0x42, 0xf4, 0x70, 0x1f, 0x3e, 0xed,
    0x90, 0x61, 0x99, 0x04, 0x32, 0xc4, 0x5b, 0x19, 0x1b, 0x88};

}  // namespace rtype::client
