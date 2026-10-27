#pragma once

#include <cstddef>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <vector>
#include "FinalPilot.hpp"
#include "Taunt.hpp"

namespace rtype::client {

/** @returns Whether a taunt may show for these pilots, best first. */
[[nodiscard]] bool tauntFits(const Taunt& taunt,
                             std::span<const FinalPilot> pilots);

/** @returns The index in TAUNTS of a taunt that fits the pilots, drawn from
 * @p random among those that fit, never @p previous. */
[[nodiscard]] std::size_t pickTaunt(std::span<const FinalPilot> pilots,
                                    std::mt19937& random,
                                    std::optional<std::size_t> previous);

/** @returns The lines of a taunt, the marks of a name replaced by the name
 * of the best, the last or the first pilot at 0. */
[[nodiscard]] std::vector<std::string> tauntLinesOf(
    const Taunt& taunt, std::span<const FinalPilot> pilots);

}  // namespace rtype::client
