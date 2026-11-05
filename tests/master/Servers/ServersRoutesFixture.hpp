#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "RouteCall.hpp"
#include "RunningMaster.hpp"
#include "ServerHeartbeat.hpp"

/**
 * @brief The master, started on a free port and a database of its own, with
 * one server created through `POST /api/servers`, and a client of its routes.
 */
class ServersRoutesFixture : public ::testing::Test {
 protected:
  ServersRoutesFixture();

  /** @returns The id of the server the fixture created. */
  [[nodiscard]] std::int64_t createdId() const;

  /** @returns The key of the server the fixture created. */
  [[nodiscard]] const std::string& key() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to a creation of "Paris 1". */
  [[nodiscard]] httplib::Result createParis() const;

  /** @returns The answer to the registration of "Paris 1" with key(). */
  [[nodiscard]] httplib::Result registerParis() const;

  /** @returns The id of a second server, created and registered as "Paris 1"
   * with a key of its own, or nothing when the master did not answer. */
  [[nodiscard]] std::optional<std::int64_t> registerSecondServer() const;

  /** @returns The answer to the heartbeat @p heartbeat sent with key(). */
  [[nodiscard]] httplib::Result sendHeartbeat(
      const rtype::network::ServerHeartbeat& heartbeat) const;

  /** @returns The answer to the games of the server @p serverId, no key sent.
   */
  [[nodiscard]] httplib::Result readGames(std::string_view serverId) const;

  /** @returns The answer to the list of servers, @p query after its path
   * (empty, or "?limit=…"), no key sent. */
  [[nodiscard]] httplib::Result readServers(std::string_view query) const;

  /** @returns The answer to @p part ("/games", "/samples" or
   * "/statuses") of the server @p serverId, no key sent. */
  [[nodiscard]] httplib::Result readServerPart(std::string_view serverId,
                                               std::string_view part) const;

 private:
  RunningMaster master_;
  std::int64_t createdId_{};
  std::string key_;
};
