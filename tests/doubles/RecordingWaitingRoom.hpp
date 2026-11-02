#pragma once

#include <optional>
#include <span>
#include <vector>
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"

/**
 * @brief Waiting room double that writes down every ready state asked for, in
 * order, and whose players and start of the game the test sets: a real room
 * changes them on its own, at any time between two updates.
 */
class RecordingWaitingRoom : public rtype::client::IWaitingRoom {
 public:
  [[nodiscard]] std::span<const rtype::network::LobbyPlayer> lobbyPlayers()
      const override;
  /** @brief Writes the value down, and keeps it as the one asked for, as a
   * real room does. */
  void setReady(bool ready) override;
  [[nodiscard]] std::optional<bool> requestedReady() const override;
  [[nodiscard]] bool gameStarted() const override;

  void setLobbyPlayers(std::vector<rtype::network::LobbyPlayer> players);
  /** @brief Sets the ready state asked for, as a request made before the test
   * would have, without writing a request down. */
  void setRequestedReady(std::optional<bool> ready);
  void setGameStarted(bool started);

  /** @returns The values setReady() was called with, in order. */
  [[nodiscard]] const std::vector<bool>& readyRequests() const;

 private:
  std::vector<rtype::network::LobbyPlayer> lobbyPlayers_;
  std::optional<bool> requestedReady_;
  bool gameStarted_ = false;
  std::vector<bool> readyRequests_;
};
