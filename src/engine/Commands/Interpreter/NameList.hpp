#pragma once

#include <span>
#include <string>

namespace rtype::engine {

/**
 * @returns @p names written on one line, separated by a comma and a space, as
 * the refusal of an unknown name lists the known ones.
 */
[[nodiscard]] std::string joinedNames(std::span<const std::string> names);

}  // namespace rtype::engine
