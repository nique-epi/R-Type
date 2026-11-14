#pragma once

#include <string_view>
#include "BossStatus.hpp"

namespace rtype::client::testing {

/** @brief A status of a boss arriving, as the server sends it. */
constexpr network::BossStatus ARRIVING_BOSS_STATUS{
    .stage = network::BossStage::Arriving};

/** @brief The level a status of the server tells: its biome, how far it
 * scrolled and how fast it scrolls. */
constexpr std::string_view STATUS_BIOME = "fosse";
constexpr float STATUS_SCROLL_POSITION = 240.0F;
constexpr float STATUS_SCROLL_SPEED = 120.0F;

}  // namespace rtype::client::testing
