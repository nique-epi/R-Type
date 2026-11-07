#pragma once

#include <cstdint>
#include <string>

namespace rtype::engine {

/**
 * @returns The whole number @p text is made of: digits, after an optional
 * leading minus.
 * @throws CommandRefusedException when it is anything else, or too large for
 * 64 bits, so the interpreter answers the line instead of running it.
 */
[[nodiscard]] std::int64_t wholeNumberFrom(const std::string& text);

}  // namespace rtype::engine
