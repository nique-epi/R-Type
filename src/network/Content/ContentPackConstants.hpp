#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "NetworkConstants.hpp"

namespace rtype::network {

/** @brief The version of the format of a content pack, its first byte. */
constexpr std::uint8_t CONTENT_PACK_FORMAT_VERSION = 1;

/**
 * @brief The largest content pack, encoded, 8 MiB: the content the game ships
 * with is about 150 KiB, and a client never keeps more than this per server.
 */
constexpr std::size_t MAXIMUM_CONTENT_PACK_BYTES = std::size_t{8} * 1024 * 1024;

/** @brief The most files a content pack holds: a sheet and a sprite sheet for
 * each of the MAXIMUM_ENTITY_TYPES types. */
constexpr std::size_t MAXIMUM_CONTENT_PACK_FILES = 64;

/** @brief The longest path of a file of a content pack, in bytes. */
constexpr std::size_t MAXIMUM_CONTENT_PATH_LENGTH = 64;

/**
 * @brief The only characters a path of a content pack may hold, so a client
 * that writes the files of a pack below its cache never writes elsewhere.
 */
constexpr std::string_view CONTENT_PATH_CHARACTERS =
    "abcdefghijklmnopqrstuvwxyz0123456789_./-";

/** @brief What separates the folders of a path of a content pack. */
constexpr char CONTENT_PATH_SEPARATOR = '/';

/** @brief The names a folder of a path may not have: the folder itself and
 * the folder above it. */
constexpr std::string_view CURRENT_FOLDER_NAME = ".";
constexpr std::string_view PARENT_FOLDER_NAME = "..";

/** @brief The bytes of the fingerprint of a content pack: a hash of 256 bits.
 */
constexpr std::size_t CONTENT_FINGERPRINT_BYTES = 32;

/**
 * @brief The context of libhydrogen the fingerprint of a pack is computed in:
 * exactly hydro_hash_CONTEXTBYTES characters, so no other hash of the project
 * gives the same value for the same bytes.
 */
constexpr std::string_view CONTENT_FINGERPRINT_CONTEXT = "rtpack01";

/** @brief The characters of a fingerprint written in lowercase hexadecimal:
 * two per byte. */
constexpr std::size_t CONTENT_FINGERPRINT_TEXT_LENGTH =
    CONTENT_FINGERPRINT_BYTES * HEX_DIGITS_PER_BYTE;

}  // namespace rtype::network
