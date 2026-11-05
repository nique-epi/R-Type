#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <string_view>
#include <vector>
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerInformationDesk.hpp"

/** @brief A desk that answers DESK_SERVER_NAME, DESK_GAME_VERSION and
 * DESK_OCCUPANCY, and records what it sends. */
class ServerInformationDeskFixture : public ::testing::Test {
 protected:
  ServerInformationDeskFixture();

  /** @brief Builds the desk again, answering @p name. */
  void answerAs(std::string_view name);

  /** @brief Gives the desk @p datagrams at @p now. */
  void answer(const std::vector<rtype::network::IncomingDatagram>& datagrams,
              std::chrono::nanoseconds now);

  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;

 private:
  std::vector<rtype::network::OutgoingDatagram> sent_;
  std::optional<rtype::server::ServerInformationDesk> desk_;
};
