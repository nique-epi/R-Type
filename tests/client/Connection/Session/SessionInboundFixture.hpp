#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "ArrivalTally.hpp"
#include "PacketHeader.hpp"
#include "Reception.hpp"
#include "RecordingDatagramLink.hpp"
#include "SessionAcknowledgements.hpp"
#include "SessionInbound.hpp"

namespace rtype::client::testing {

/** @brief The inbound side of a session over a recording link. */
class SessionInboundFixture : public ::testing::Test {
 protected:
  /** @brief Opens the session with the token SESSION_TOKEN, its acceptance
   * carrying ACCEPTANCE_SEQUENCE. */
  void openSession();

  /** @returns What the inbound side makes of a datagram carrying @p token
   * and FIRST_SEQUENCE. */
  [[nodiscard]] Reception receiveWithToken(std::uint64_t token);

  /** @returns What the inbound side makes of a datagram carrying
   * SESSION_TOKEN and @p sequence. */
  [[nodiscard]] Reception receiveWithSequence(std::uint16_t sequence);

  [[nodiscard]] std::size_t wrongTokenCount() const;

 private:
  [[nodiscard]] Reception receive(const network::PacketHeader& header);

  RecordingDatagramLink link_;
  ArrivalTally arrivals_;
  SessionAcknowledgements acknowledgements_{arrivals_};
  SessionInbound inbound_{link_, acknowledgements_};
};

}  // namespace rtype::client::testing
