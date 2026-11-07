#pragma once

#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "AdminConsoleCommands.hpp"
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "GameRegistry.hpp"
#include "LatestServerMeasures.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingConsoleOutput.hpp"
#include "RecordingDatagramSender.hpp"
#include "RejectionLedger.hpp"
#include "TrafficCounters.hpp"

/** @brief The commands of the console registered in an interpreter of their
 * own, over a registry, a sender and an output the tests read. */
class AdminConsoleCommandsFixture : public ::testing::Test {
 protected:
  AdminConsoleCommandsFixture();

  /** @returns What the interpreter did with @p line. */
  [[nodiscard]] rtype::engine::CommandResult run(std::string_view line);

  /** @brief Opens game GAME_THREE, which takes place 1. */
  void openGameThree();

  /** @brief Has the ledger count two rejected datagrams. */
  void rejectTwoDatagrams();

  /** @brief Registers the commands again in the same interpreter. */
  void registerAgain();

  [[nodiscard]] const std::vector<rtype::network::OutgoingDatagram>& sent()
      const;
  [[nodiscard]] std::vector<std::string> lines() const;

 private:
  rtype::engine::CommandInterpreter interpreter_;
  rtype::server::GameRegistry registry_;
  RecordingDatagramSender sender_;
  rtype::network::TrafficCounters traffic_;
  rtype::server::RejectionLedger rejections_;
  rtype::server::LatestServerMeasures measures_;
  RecordingConsoleOutput output_;
  rtype::server::AdminConsoleCommands commands_;
};
