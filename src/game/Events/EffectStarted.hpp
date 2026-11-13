#pragma once

#include <cstdint>
#include <string>

namespace rtype::game {

/** @brief Event published when a body plays once the effect its sheet names
 * `effect`, in `[appearance.effects]`. The network id is the one of its
 * NetworkIdentity. */
struct EffectStarted {
  std::uint32_t networkId{};
  std::string effect;

  bool operator==(const EffectStarted&) const = default;
};

}  // namespace rtype::game
