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
 * Building it opens nothing; start() builds the client and the thread, stop()
 * ends the thread, a call in progress ended within the timeouts of
 * MasterClient. The asks and the readings are safe from both threads.
 */
class MasterLink final : public IMasterReads {
 public:
  explicit MasterLink(network::MasterAccess access);
  ~MasterLink() override = default;
  MasterLink(const MasterLink&) = delete;
  MasterLink& operator=(const MasterLink&) = delete;
  MasterLink(MasterLink&&) = delete;
  MasterLink& operator=(MasterLink&&) = delete;

  /** @throws network::InvalidMasterUrlException when the url is not
   * `http://<host>[:<port>]`; the thread does not start then. */
  void start();

  /** @brief Ends the thread, then forgets the client. */
  void stop();

  std::uint64_t askServers() override;
  std::uint64_t askGames(std::int64_t serverId) override;
  [[nodiscard]] std::optional<ServerListReading> servers() const override;
  [[nodiscard]] std::optional<GameListReading> games() const override;

 private:
  /** @brief The loop of MasterLink: waits for a newer ask, serves it. */
  void run(const std::stop_token& stop);
  /** @brief Serves each kind of @p asks newer than @p served, then marks it
   * served. */
  void serve(const MasterAsks& asks, MasterAsks& served);

  network::MasterAccess access_;
  MasterAsks asked_;
  concurrency::LatestValue<MasterAsks> asks_;
  concurrency::LatestValue<ServerListReading> servers_;
  concurrency::LatestValue<GameListReading> games_;
  std::unique_ptr<network::MasterClient> client_;
  std::jthread thread_;
};

}  // namespace rtype::client
