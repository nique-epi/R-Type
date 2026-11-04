#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <deque>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "InputKeeper.hpp"
#include "PlayerRoster.hpp"
#include "SeededLoss.hpp"
#include "SessionsFixtures.hpp"
#include "UnconfirmedInputs.hpp"

namespace rtype::server::testing {

/**
 * @brief A client that numbers its inputs with UnconfirmedInputs, a network
 * that loses datagrams at random in both directions, and the InputKeeper of a
 * server that confirms what it applied now and then.
 */
class InputRedundancyFixture : public ::testing::Test {
 protected:
  InputRedundancyFixture();

  /** @brief Sends @p count inputs, one per step, through the lossy network. */
  void sendInputs(std::uint32_t count);

  /** @returns Every input the server showed the game, in order. */
  [[nodiscard]] const std::vector<ShownInput>& shown() const;

 private:
  void sendInput(std::uint32_t number);
  void confirmWhenDue(std::uint32_t step);

  PlayerRoster roster_;
  const ConnectedPlayer& player_;
  std::vector<ShownInput> shown_;
  InputKeeper keeper_;
  client::UnconfirmedInputs sender_;
  SeededLoss loss_;
  std::deque<std::pair<std::uint32_t, std::uint32_t>> confirmationsInFlight_;
};

}  // namespace rtype::server::testing
