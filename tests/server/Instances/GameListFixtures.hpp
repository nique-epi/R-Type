#pragma once

#include <memory>
#include "EventBus.hpp"
#include "GameInstance.hpp"

namespace rtype::server::testing {

/**
 * @returns A game that sends nothing anywhere, playing testWavePlan() from
 * SOME_SEED and publishing on @p events, which must outlive it.
 */
[[nodiscard]] std::unique_ptr<GameInstance> silentGame(
    engine::EventBus& events);

}  // namespace rtype::server::testing
