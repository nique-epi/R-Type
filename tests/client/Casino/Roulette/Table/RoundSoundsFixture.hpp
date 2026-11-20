#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <vector>
#include "CasinoRoomState.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RoundSounds.hpp"

/** @brief The sounds of a wheel, followed frame after frame over the rooms
 * of the roulette fixtures, with a recording sound player. */
class RoundSoundsFixture : public ::testing::Test {
 protected:
  /** @brief Shows the wheel @p room, as one frame of the table. */
  void see(const rtype::network::CasinoRoomState& room);
  /** @brief Shows the wheel @p room to a player who asked for less
   * motion. */
  void seeWithLessMotion(const rtype::network::CasinoRoomState& room);
  /** @returns The room of the fixtures spinning, @p left before its
   * result. */
  [[nodiscard]] static rtype::network::CasinoRoomState spinningWith(
      std::chrono::seconds left);
  /** @returns The ids of the sounds played, the first played first. */
  [[nodiscard]] const std::vector<std::string>& played() const;

 private:
  RecordingSoundPlayer sounds_;
  rtype::client::RoundSounds wheel_{sounds_};
};
