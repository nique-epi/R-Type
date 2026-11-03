#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The name of each message of the protocol page, in the order of the
 * values of MessageType, the first one being FIRST_NAMED_MESSAGE_TYPE.
 */
constexpr std::size_t FIRST_NAMED_MESSAGE_TYPE =
    static_cast<std::size_t>(MessageType::ConnectionRequest);
constexpr auto MESSAGE_TYPE_NAMES = std::to_array<std::string_view>(
    {"ConnectionRequest", "ConnectionAccepted", "ConnectionRefused",
     "Disconnection", "Heartbeat", "ReadyState", "LobbyState", "GameStarted",
     "PlayerInput", "WorldState", "EntitySpawned", "EntityDestroyed",
     "GameEvent", "PlayerLeft", "GameEnded", "ConnectionChallenge",
     "GameLoaded", "LeaderboardRequest", "LeaderboardAnswer"});

static_assert(FIRST_NAMED_MESSAGE_TYPE + MESSAGE_TYPE_NAMES.size() - 1 ==
                  static_cast<std::size_t>(MessageType::LeaderboardAnswer),
              "every message up to LeaderboardAnswer has its name");

/**
 * @brief The figures a DirectionTraffic keeps per type of message: one slot per
 * message, at its type byte, and UNKNOWN_MESSAGE_SLOT for every byte no message
 * has, so a forged byte never adds more than one figure.
 */
constexpr std::size_t UNKNOWN_MESSAGE_SLOT = 0;
constexpr std::size_t MESSAGE_SLOT_COUNT =
    FIRST_NAMED_MESSAGE_TYPE + MESSAGE_TYPE_NAMES.size();

static_assert(UNKNOWN_MESSAGE_SLOT < FIRST_NAMED_MESSAGE_TYPE,
              "no message holds the slot of the unknown messages");

}  // namespace rtype::network
