#pragma once

#include <span>
#include <string>
#include <string_view>
#include "EngineException.hpp"

namespace rtype::server {

/** @returns "expected: <name> <arguments>", the refusal of a line with the
 * wrong words for the command @p name. */
[[nodiscard]] engine::CommandRefusedException usageRefusal(
    std::string_view name, std::string_view arguments);

/** @throws engine::CommandRefusedException "expected: <command>" when
 * @p words is not empty. */
void requireNoWord(std::span<const std::string> words,
                   std::string_view command);

}  // namespace rtype::server
