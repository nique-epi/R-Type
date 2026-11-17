#pragma once

#include <array>
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "EventField.hpp"

namespace rtype::server {

/** @brief How many fields name a player in an event of the journal. */
constexpr std::size_t PLAYER_EVENT_FIELD_COUNT = 3;

/**
 * @returns The fields that name @p player in an event of the journal: its
 * number, its address and its port. The address points into @p player, which
 * must outlive them.
 */
[[nodiscard]] std::array<logging::EventField, PLAYER_EVENT_FIELD_COUNT>
playerEventFields(const ConnectedPlayer& player);

}  // namespace rtype::server
