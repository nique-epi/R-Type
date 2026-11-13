#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include "BotLaunchConstants.hpp"
#include "BotPilotKind.hpp"
#include "ClientConstants.hpp"
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"

namespace rtype::tools {

/** @brief What the arguments of the tool ask: how many bots join which server,
 * for how long, from which seed and flown by what, or the usage. */
struct BotLaunch {
  std::size_t botCount{DEFAULT_BOT_COUNT};
  network::Endpoint server{
      .address = std::string{client::DEFAULT_SERVER_ADDRESS},
      .port = network::DEFAULT_SERVER_PORT};
  std::chrono::seconds duration{DEFAULT_RUN_DURATION};
  std::uint32_t seed{DEFAULT_BOT_SEED};
  BotPilotKind pilot{BotPilotKind::Random};
  bool helpRequested{false};
};

}  // namespace rtype::tools
