#pragma once

#include <gtest/gtest.h>
#include <functional>
#include <string>
#include "CreatedServer.hpp"
#include "MasterClient.hpp"
#include "NetworkException.hpp"
#include "RunningMaster.hpp"

/** @brief A running master, and clients that call it as a game server does. */
class MasterClientFixture : public ::testing::Test {
 protected:
  /** @returns A client of the master that sends @p key. */
  [[nodiscard]] rtype::network::MasterClient clientWith(
      const std::string& key) const;

  [[nodiscard]] const RunningMaster& master() const;

  /** @returns The server "Paris 1", created through a client with no key. */
  [[nodiscard]] rtype::network::CreatedServer createdParis() const;

  /**
   * @returns The MasterRefusedException @p call throws, or a refusal of
   * NO_REFUSAL status when it throws none.
   */
  [[nodiscard]] static rtype::network::MasterRefusedException refusalOf(
      const std::function<void()>& call);

 private:
  RunningMaster master_;
};
