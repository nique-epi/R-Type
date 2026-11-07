#pragma once

#include <functional>
#include "CommandInterpreter.hpp"
#include "GameRegistry.hpp"
#include "IConsoleOutput.hpp"
#include "IDatagramSender.hpp"
#include "LatestServerMeasures.hpp"
#include "RejectionLedger.hpp"
#include "TrafficCounters.hpp"

namespace rtype::server {

/** @brief What the commands of the console read and act on; each must
 * outlive the commands. */
struct AdminConsoleParts {
  /** @brief The games of the supervisor. */
  std::reference_wrapper<const GameRegistry> games;
  /** @brief Where close-game queues the stop orders. */
  std::reference_wrapper<IDatagramSender> sender;
  /** @brief What the socket of the supervisor sent and received. */
  std::reference_wrapper<const network::TrafficCounters> traffic;
  /** @brief The datagrams the network thread rejected. */
  std::reference_wrapper<const RejectionLedger> rejections;
  /** @brief The newest measures of the games. */
  std::reference_wrapper<const LatestServerMeasures> measures;
  /** @brief The table help lists. */
  std::reference_wrapper<const engine::CommandInterpreter> interpreter;
  /** @brief Where the answers go. */
  std::reference_wrapper<IConsoleOutput> output;
};

}  // namespace rtype::server
