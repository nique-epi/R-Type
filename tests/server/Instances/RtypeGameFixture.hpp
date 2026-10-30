#pragma once

#include <cstddef>
#include "GameModuleFixture.hpp"
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "ReplicationModule.hpp"
#include "RtypeRulesModule.hpp"

namespace rtype::server::testing {

/**
 * @brief The parts of a game of R-Type: the replication module and the rules
 * module, installed and started in the order a game lists them, over the
 * context of GameModuleFixture.
 */
class RtypeGameFixture : public GameModuleFixture {
 protected:
  /** @brief Installs the replication module alone. */
  void installReplication();

  /** @brief Installs the replication module, tells the replication about the
   * world of the fixture and gives the replication. */
  IGameReplication& replicationOfTheWorld();

  /** @brief Installs the replication module, then the rules, and starts both
   * in that order. */
  void startBoth();

  /** @brief Stops the rules, then the replication, as a game stops them. */
  void stopBoth();

  [[nodiscard]] ReplicationModule& replicationModule();
  [[nodiscard]] RtypeRulesModule& rules();

  /** @brief Checks that something was sent to the player at @p index, and that
   * the first message sent to it is of type @p type. */
  void expectFirstMessageSentTo(std::size_t index, network::MessageType type);

 private:
  ReplicationModule replicationModule_;
  RtypeRulesModule rules_;
};

}  // namespace rtype::server::testing
