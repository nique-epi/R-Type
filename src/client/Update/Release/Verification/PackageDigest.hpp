#pragma once

#include <cstddef>
#include <span>
#include <string>

namespace rtype::client {

/** @returns The SHA-256 of @p bytes in lowercase hexadecimal, as Python's
 * hashlib writes it; empty if OpenSSL could not compute it. */
[[nodiscard]] std::string sha256HexadecimalOf(std::span<const std::byte> bytes);

}  // namespace rtype::client
