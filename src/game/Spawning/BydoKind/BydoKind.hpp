#pragma once

#include <cstdint>
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

}  // namespace rtype::game
