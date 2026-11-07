#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game {

/**
 * @brief One attack of the Bydo Tyrant: its number, where its three timings
 * start among the settings of its behaviour (signal, strike, recover in a
 * row, see timingsAt()), and whether `rage_signal_percent` shortens its
 * signal: the enraged variant of a calm attack.
 */
struct TyrantAttack {
  std::uint8_t attack{0};
  std::size_t signalSetting{0};
  bool hastened{false};
};

}  // namespace rtype::game
