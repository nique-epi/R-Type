#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

/** @brief The random bytes of a server key, 256 bits. */
constexpr std::size_t SERVER_KEY_BYTES = 32;

/** @brief The hexadecimal digits that write one byte, and a whole key. */
constexpr std::size_t HEX_DIGITS_PER_BYTE = 2;
constexpr std::size_t SERVER_KEY_TEXT_SIZE =
    SERVER_KEY_BYTES * HEX_DIGITS_PER_BYTE;

/** @brief The byte hydro_bin2hex() writes after the digits. */
constexpr std::size_t HEX_TERMINATOR_SIZE = 1;

/** @brief The digits of a key as the master writes them, lowercase. */
constexpr std::string_view SERVER_KEY_DIGITS = "0123456789abcdef";

/**
 * @brief The context of libhydrogen the fingerprint of a key is computed in:
 * exactly hydro_hash_CONTEXTBYTES characters, so no other hash of the master
 * can give the same value.
 */
constexpr std::string_view FINGERPRINT_CONTEXT = "srvkeys1";

/** @brief The statements of the table of keys, and their parameters. */
constexpr std::string_view INSERT_SERVER_KEY_STATEMENT = R"sql(
INSERT INTO server_keys (name, fingerprint, issued_at)
VALUES (:name, :fingerprint, :issued_at)
)sql";
constexpr std::string_view SERVER_KEY_OF_FINGERPRINT_QUERY =
    "SELECT id FROM server_keys WHERE fingerprint = :fingerprint";
constexpr std::string_view KEY_NAME_PARAMETER = ":name";
constexpr std::string_view FINGERPRINT_PARAMETER = ":fingerprint";
constexpr std::string_view ISSUED_AT_PARAMETER = ":issued_at";

/** @brief The column of the id in SERVER_KEY_OF_FINGERPRINT_QUERY. */
constexpr int KEY_ID_COLUMN = 0;

}  // namespace rtype::master
