#pragma once

#include <string>
#include "LobbyScreenFixture.hpp"

/** @brief The waiting room after the connection, its canvas at the largest
 * text size. */
class LargestLobbyFixture : public LobbyScreenFixture {
 protected:
  LargestLobbyFixture();

  /** @brief The server lists four pilots of the longest nickname, the local
   * player first, and a frame goes by. */
  void listFourLongestPilots();

  /** @brief Turns the wheel of the pointer by @p notches, down for a negative
   * count, then a frame goes by. */
  void turnTheWheel(float notches);

  /** @returns What the first bay writes on the rows of its nickname, every
   * line one after the other. */
  [[nodiscard]] std::string nicknameOfTheFirstBay() const;
};
