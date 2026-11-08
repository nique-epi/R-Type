#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

namespace rtype::network {

/** @returns @p bytes as the text a file or the body of an answer carries,
 * byte for byte. */
[[nodiscard]] std::string textOf(std::span<const std::byte> bytes);

/** @returns The bytes of @p text, as a view of it: valid while @p text is. */
[[nodiscard]] std::span<const std::byte> bytesOf(std::string_view text);

}  // namespace rtype::network
