#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/** @brief An input DelayedInputs holds, and the tick it comes due. */
struct HeldInput {
  std::uint8_t playerNumber{};
  network::PlayerInput input{};
  std::uint64_t dueTick{};
};

/**
 * @brief Holds each input of every player for the same number of ticks before
 * it is applied, so all players lag the same amount behind their keys.
 *
 * An input pushed during tick N comes due in tick N + delay; with a delay of 0
 * it is due in the tick that pushed it. Each release() gives one due input of
 * each player, its oldest, as the client predicts one input a tick: two inputs
 * read in one tick come out in two ticks, and a tick with none applies none,
 * the ship keeping the input before. An input that waited more than
 * INPUT_CATCH_UP_TICKS past its tick comes out with the next one, so a player
 * whose inputs arrive faster than the ticks catches up; each tick without an
 * input of a player, and each catch-up, is counted in its InputPace. The delay
 * counts from
 * the tick the server read the input, not from the moment the player pressed
 * the key, so it adds to the ping of each player instead of replacing it.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class DelayedInputs {
 public:
  /** @brief What release() is given each input due, with its player. */
  using InputSink =
      std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>;

  /**
   * @param delayTicks Ticks an input is held, from LOWEST_INPUT_DELAY_TICKS to
   * HIGHEST_INPUT_DELAY_TICKS.
   * @throws InvalidInputDelayException when it is outside that range.
   */
  explicit DelayedInputs(std::size_t delayTicks);

  /** @brief Holds @p input of the player numbered @p playerNumber. */
  void push(std::uint8_t playerNumber, const network::PlayerInput& input);

  /**
   * @brief Gives @p sink the oldest due input of each player, two when that
   * one waited past INPUT_CATCH_UP_TICKS, then moves to the next tick; call
   * it once per tick. An input whose player left the roster is dropped.
   */
  void release(PlayerRoster& roster, const InputSink& sink);

  /** @brief Drops every input held for the player numbered @p playerNumber. */
  void clear(std::uint8_t playerNumber);

 private:
  /** @returns How many inputs of @p held come out this tick: none when its
   * oldest is not due, two when it waited past INPUT_CATCH_UP_TICKS and the
   * next one is due too, one otherwise. */
  [[nodiscard]] std::size_t dueCount(const std::deque<HeldInput>& held) const;

  std::size_t delayTicks_;
  std::uint64_t currentTick_{0};
  std::array<std::deque<HeldInput>, network::MAXIMUM_PLAYERS> held_;
};

}  // namespace rtype::server
