#pragma once

#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns How many letters of a text of @p length letters are written
 * @p intoTyping after its first letter appeared, one more every
 * @p letterTime: none before, the first at once, the whole text at most.
 */
[[nodiscard]] std::size_t lettersTypedAt(engine::Duration intoTyping,
                                         engine::Duration letterTime,
                                         std::size_t length);

/** @returns How long @p letters letters take to type, @p letterTime each. */
[[nodiscard]] engine::Duration typingTimeOf(engine::Duration letterTime,
                                            std::size_t letters);

/** @returns How many letters a UTF-8 text holds. */
[[nodiscard]] std::size_t letterCountOf(std::string_view text);

/** @returns The first @p letters letters of a UTF-8 text, never a character
 * cut in two; the whole text when it holds fewer. */
[[nodiscard]] std::string_view typedPrefix(std::string_view text,
                                           std::size_t letters);

}  // namespace rtype::client
