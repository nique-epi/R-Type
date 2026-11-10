#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief Two files of a pack, in the order of their paths. */
constexpr std::string_view DRIFTER_SHEET_PATH = "entities/drifter.toml";
constexpr std::string_view DRIFTER_SHEET_BYTES = "[gameplay]\n[appearance]\n";
constexpr std::string_view BYDOMANCER_SPRITES_PATH =
    "sprites/mobs/bydomancer.png";
constexpr std::string_view BYDOMANCER_PIXELS = "\x89PNG\r\n\x1a\npixels";

/** @brief The bytes of an empty pack: its version (u8) and its count of files
 * (u16). */
constexpr std::size_t EMPTY_PACK_BYTES = 3;

/**
 * @brief What a pack of one file named ONE_LETTER_PATH takes besides the bytes
 * of the file: the version (1 byte), the count (2), the length of the path
 * (1), the path (1) and the size of the file (4).
 */
constexpr std::string_view ONE_LETTER_PATH = "a";
constexpr std::size_t ONE_FILE_PACK_OVERHEAD = 9;

/** @brief The largest pack, 8 MiB, and the most files, 510, a pack may hold,
 * as the page of the protocol states them. */
constexpr std::size_t LARGEST_PACK_BYTES = std::size_t{8} * 1024 * 1024;
constexpr std::size_t MOST_PACK_FILES = 510;

/** @brief What the files of filesNumbered() are: `f` and three decimal
 * digits, each holding ONE_BYTE. */
constexpr std::string_view NUMBERED_FILE_PREFIX = "f";
constexpr std::size_t FILE_NUMBER_DIGITS = 3;
constexpr std::size_t DECIMAL_BASE = 10;
constexpr char ONE_BYTE = 'x';

/** @brief The version of the format every program reads, and the one file of
 * one byte of a pack written by hand. */
constexpr std::uint8_t READ_FORMAT_VERSION = 1;
constexpr std::uint16_t ONE_FILE = 1;
constexpr std::uint32_t ONE_FILE_BYTE = 1;

/** @brief A version of the format no program reads. */
constexpr std::uint8_t UNKNOWN_FORMAT_VERSION = 2;

/** @brief Paths no pack may name, each for another reason. */
constexpr std::array REFUSED_PATHS{
    std::string_view{""},
    std::string_view{"../escape.png"},
    std::string_view{"/root.png"},
    std::string_view{"sprites/../../escape.png"},
    std::string_view{"sprites//double.png"},
    std::string_view{"sprites/"},
    std::string_view{"sprites/./here.png"},
    std::string_view{"Sprites/upper.png"},
    std::string_view{"sprites\\back.png"},
    std::string_view{
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"},
};

/** @brief The longest path a pack may name: 64 letters. */
constexpr std::string_view LONGEST_PATH =
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

/** @brief A pack of one file decodes to one file. */
constexpr std::size_t ONE_DECODED_FILE = 1;

/** @brief The bit a test flips in a byte of a pack, and the byte it adds
 * after a pack. */
constexpr std::byte FLIPPED_BIT{1};
constexpr std::byte EXTRA_BYTE{0};

/** @brief The digits a fingerprint is written with. */
constexpr std::string_view LOWERCASE_DIGITS = "0123456789abcdef";

/** @brief A fingerprint written in hexadecimal is 64 lowercase digits. */
constexpr std::size_t FINGERPRINT_TEXT_LENGTH = 64;

}  // namespace rtype::network::testing
