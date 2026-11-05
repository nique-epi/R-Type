#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace rtype::game {

/**
 * @brief The kinds of Bydo; each one is piloted by its own behaviour.
 */
enum class BydoKind : std::uint8_t {
  Drifter,
  Weaver,
};

/**
 * @returns The name of the behaviour that pilots the Bydos of @p kind: Drifter
 * goes straight, Weaver weaves.
 */
[[nodiscard]] std::string_view behaviourNameOf(BydoKind kind);

/**
 * @returns The points the player who destroys a Bydo of @p kind earns.
 */
[[nodiscard]] std::uint32_t pointsOf(BydoKind kind);

/**
 * @returns The values the behaviour of the Bydos of @p kind is given for its
 * settings, in the order it declares them: heading -1 for a Drifter, amplitude
 * 48 and period 2000 ms for a Weaver. The array outlives every Bydo.
 */
[[nodiscard]] std::span<const std::int64_t> behaviourSettingsOf(BydoKind kind);

}  // namespace rtype::game
