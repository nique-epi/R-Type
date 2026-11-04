#pragma once

#include <optional>
#include "BotStanding.hpp"
#include "ConnectionPhase.hpp"

namespace rtype::tools::testing {

constexpr BotStanding ACCEPTED_AND_WAITING{
    .phase = client::ConnectionPhase::Accepted,
    .gameStarted = false,
    .requestedReady = std::nullopt};
constexpr BotStanding ALREADY_ASKED{.phase = client::ConnectionPhase::Accepted,
                                    .gameStarted = false,
                                    .requestedReady = true};
constexpr BotStanding ASKED_NOT_READY{
    .phase = client::ConnectionPhase::Accepted,
    .gameStarted = false,
    .requestedReady = false};
constexpr BotStanding IN_A_GAME{.phase = client::ConnectionPhase::Accepted,
                                .gameStarted = true,
                                .requestedReady = std::nullopt};
constexpr BotStanding REFUSED{.phase = client::ConnectionPhase::Refused,
                              .gameStarted = false,
                              .requestedReady = std::nullopt};

constexpr bool EVERY_BOT_ANSWERED = false;
constexpr bool A_BOT_STILL_CONNECTING = true;

}  // namespace rtype::tools::testing
