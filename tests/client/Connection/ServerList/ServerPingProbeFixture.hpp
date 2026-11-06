#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "Endpoint.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerPingProbe.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A probe over a recording link, its clock at ROUND_SENT_AT: the tests
 * start rounds and deliver the answers of the servers. */
class ServerPingProbeFixture : public ::testing::Test {
 protected:
  ServerPingProbeFixture();

  [[nodiscard]] static network::Endpoint paris();
  [[nodiscard]] static network::Endpoint lyon();
  [[nodiscard]] static network::Endpoint stranger();

  /** @brief Starts a round to Paris and Lyon. */
  void measureParisAndLyon();

  /** @returns The sentAt the requests of the round carry. */
  [[nodiscard]] std::uint32_t sentAtOfTheRound() const;

  /** @brief Delivers an answer of @p server echoing @p sentAt, held
   * HELD_FOR_MICROSECONDS, arriving @p after the round was sent. */
  void deliverAnswer(const network::Endpoint& server, std::uint32_t sentAt,
                     engine::Duration after);

  /** @brief Delivers bytes from Paris that are no datagram of the protocol. */
  void deliverGarbageFromParis();

  [[nodiscard]] ServerPingProbe& probe();
  [[nodiscard]] const RecordingDatagramLink& link() const;

 private:
  RecordingDatagramLink link_;
  SimulatedClock clock_;
  ServerPingProbe probe_{link_, clock_};
};

}  // namespace rtype::client::testing
