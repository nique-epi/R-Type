#pragma once

#include "BossStatus.hpp"

namespace rtype::client::testing {

/** @brief A status of a boss arriving, as the server sends it. */
constexpr network::BossStatus ARRIVING_BOSS_STATUS{
    .stage = network::BossStage::Arriving};

}  // namespace rtype::client::testing
