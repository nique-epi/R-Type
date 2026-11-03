#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "MessageType.hpp"

namespace rtype::network {

/** @brief Module name the traffic of a program writes its log lines under. */
constexpr std::string_view TRAFFIC_LOGGER_NAME = "Traffic";

/** @brief How often the traffic goes to its file, and to the journal. */
constexpr std::chrono::seconds TRAFFIC_CSV_PERIOD{1};
constexpr std::chrono::seconds TRAFFIC_LINE_PERIOD{10};

/** @brief The first line of a traffic file, and the digits after the point of
 * its first column. */
constexpr std::string_view TRAFFIC_CSV_HEADER =
    "seconds,peer,direction,message,count,bytes";
constexpr int TRAFFIC_CSV_SECONDS_DECIMALS = 3;

/** @brief The texts of the columns of a traffic file that are not a peer's
 * address or the name of a message. */
constexpr std::string_view OTHERS_PEER_TEXT = "others";
constexpr std::string_view DEPARTED_PEER_TEXT = "departed";
constexpr std::string_view SENT_DIRECTION_TEXT = "sent";
constexpr std::string_view RECEIVED_DIRECTION_TEXT = "received";
constexpr std::string_view ALL_MESSAGES_TEXT = "all";
constexpr std::string_view UNREADABLE_MESSAGES_TEXT = "unreadable";

/** @brief What a traffic file calls the messages of a type byte no message
 * has. */
constexpr std::string_view UNKNOWN_MESSAGE_TEXT = "unknown";

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
     "GameLoaded"});

static_assert(FIRST_NAMED_MESSAGE_TYPE + MESSAGE_TYPE_NAMES.size() - 1 ==
                  static_cast<std::size_t>(MessageType::GameLoaded),
              "every value of MessageType up to GameLoaded has its name");

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
