#pragma once

#include <cstdint>
#include <string_view>
#include "GameStatus.hpp"

namespace rtype::server {

/** @brief Where a game stands, as its supervisor knows it. */
enum class GameLifecycle : std::uint8_t {
  /** @brief Launched, nothing heard from it yet. */
  Starting,
  WaitingForPlayers,
  InProgress,
  Finished,
};

/** @returns The words the journal uses for @p lifecycle. */
[[nodiscard]] std::string_view lifecycleText(GameLifecycle lifecycle);

/** @returns The lifecycle a game that reports @p state is in. */
[[nodiscard]] GameLifecycle lifecycleOf(network::GameStatusState state);

}  // namespace rtype::server
