#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include "GameListReading.hpp"
#include "IMasterReads.hpp"
#include "LatestValue.hpp"
#include "MasterAccess.hpp"
#include "MasterAsks.hpp"
#include "MasterClient.hpp"
#include "ServerListReading.hpp"

namespace rtype::client {

/**
 * @brief The reads of the master on a thread of their own, MasterLink:
 * MasterClient blocks for as long as HTTP takes, MASTER_TRANSFER_TIMEOUT at
 * most per call, and the frame loop must never wait on it.
 *
 * The frame loop publishes its asks; the thread sleeps until a newer one,
 * serves the newest of each kind only, and publishes what the master answered.
 * A master that does not answer, refuses or writes what cannot be read gives a
 * reading with no listing, so the list says so instead of waiting for ever.
 *
 * Building it opens nothing; start() builds the client of the master, which
 * checks its url, and starts the thread, which owns it; stop() ends the thread,
 * a call in progress ended within the timeouts of MasterClient. retarget()
 * names another master while the thread runs, without waiting for it: the
 * thread builds the client of that master before it serves the next ask, and a
 * call in progress ends on the master it began with, its reading answering an
 * older ask. The asks, the master and the readings are safe from both threads.
 */
class MasterLink final : public IMasterReads {
 public:
  MasterLink() = default;
  ~MasterLink() override = default;
  MasterLink(const MasterLink&) = delete;
  MasterLink& operator=(const MasterLink&) = delete;
  MasterLink(MasterLink&&) = delete;
  MasterLink& operator=(MasterLink&&) = delete;

  /** @brief Starts the thread on the master @p access names.
   * @throws network::InvalidMasterUrlException when the url is not
   * `http(s)://<host>[:<port>]`; the thread does not start then. */
  void start(network::MasterAccess access);

  /** @brief Names the master the thread reads from its next ask on; a url
   * MasterClient refuses leaves it on the master it had. */
  void retarget(network::MasterAccess access);

  /** @brief Ends the thread, and with it its client. */
  void stop();

  std::uint64_t askServers() override;
  std::uint64_t askGames(std::int64_t serverId) override;
  [[nodiscard]] std::optional<ServerListReading> servers() const override;
  [[nodiscard]] std::optional<GameListReading> games() const override;

 private:
  /** @brief The loop of MasterLink: waits for a newer ask, serves it with
   * @p client, or with the client of a master retarget() named since. */
  void run(const std::stop_token& stop,
           std::unique_ptr<network::MasterClient> client);
  /** @brief Serves each kind of @p asks newer than @p served with @p client,
   * then marks it served. */
  void serve(network::MasterClient& client, const MasterAsks& asks,
             MasterAsks& served);

  MasterAsks asked_;
  concurrency::LatestValue<MasterAsks> asks_;
  concurrency::LatestValue<ServerListReading> servers_;
  concurrency::LatestValue<GameListReading> games_;
  /** @brief The master the thread reads, newest first. */
  concurrency::LatestValue<network::MasterAccess> access_;
  std::jthread thread_;
};

}  // namespace rtype::client
