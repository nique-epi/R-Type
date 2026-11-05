#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "IMasterReads.hpp"
#include "IServerCatalog.hpp"
#include "IServerPingProbe.hpp"
#include "ServerListReading.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerRow.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The list of servers of the server map: refresh() asks the master;
 * once it answers, a round of pings goes to every server it lists online or
 * degraded and of this version, and the round closes when each answered or
 * PING_GIVE_UP_TIME passed. Only then does the snapshot change, all at once:
 * the servers that answered with their ping, those of another version with
 * none, never the servers that did not answer, which are unreachable.
 *
 * A master that did not answer gives MasterUnreachable and no server, even if
 * a list was shown before: an old list would be joined for nothing. Only the
 * reading of the newest ask counts. It runs on the thread of the frame loop,
 * where update() is called.
 */
class ServerCatalog final : public IServerCatalog {
 public:
  /** @param master, pings They must outlive the catalog. */
  ServerCatalog(IMasterReads& master, IServerPingProbe& pings);

  void refresh() override;
  [[nodiscard]] const ServerListSnapshot& snapshot() const override;

  /** @brief Takes the reading of the master once it came, the pings that came,
   * and closes the round when it is over. */
  void update(engine::Duration elapsed);

 private:
  /** @brief Starts the round of the servers @p reading lists, or says the
   * master is unreachable. */
  void takeListing(const ServerListReading& reading);
  /** @brief Gives each server that answered its ping. */
  void takeAnswers();
  /** @brief Shows the servers measured, and ends the round. */
  void closeRound();

  IMasterReads* master_;
  IServerPingProbe* pings_;
  std::uint64_t asked_{};
  std::uint64_t taken_{};
  std::optional<engine::Duration> roundAge_;
  std::vector<ServerRow> waiting_;
  std::vector<ServerRow> measured_;
  ServerListSnapshot snapshot_;
};

}  // namespace rtype::client
