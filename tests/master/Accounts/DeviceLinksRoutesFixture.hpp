#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include <string_view>
#include "PlayerLinkCall.hpp"
#include "RouteCall.hpp"
#include "RunningMaster.hpp"

/** @brief The master, started on a free port, a database of its own and the
 * test key of the passwords, and a client of the routes of its device links,
 * every call over HTTPS. */
class DeviceLinksRoutesFixture : public ::testing::Test {
 protected:
  /** @returns The master the tests call. */
  [[nodiscard]] const RunningMaster& master() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to a game asking for a link. */
  [[nodiscard]] httplib::Result createLink() const;

  /** @returns The token of a session of ACE, created then signed in; empty
   * when the master refused either. */
  [[nodiscard]] std::string aceToken() const;

  /** @returns The answer to the game of @p secret reading its link. */
  [[nodiscard]] httplib::Result readOwnLink(std::string_view secret) const;

  /** @returns The answer to the game of @p secret trading its link for a
   * session. */
  [[nodiscard]] httplib::Result tradeLink(std::string_view secret) const;

  /** @returns The answer to the game of @p secret forgetting its link. */
  [[nodiscard]] httplib::Result forgetLink(std::string_view secret) const;

  /** @returns The answer to the session of @p call reading the link of its
   * short code; its body is not sent. */
  [[nodiscard]] httplib::Result readLink(const PlayerLinkCall& call) const;

  /** @returns The answer to the session of @p call sending its body as its
   * decision on the link of its short code. */
  [[nodiscard]] httplib::Result decide(const PlayerLinkCall& call) const;

 private:
  RunningMaster master_;
};
