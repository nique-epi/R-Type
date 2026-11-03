#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

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
 * An input pushed during tick N is released by the release() of tick N + delay,
 * oldest first, whichever player sent it; with a delay of 0 it is released by
 * the release() of the tick that pushed it. The delay counts from the tick the
 * server read the input, not from the moment the player pressed the key, so it
 * adds to the ping of each player instead of replacing it.
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
   * @brief Gives @p sink each held input now due, oldest first, then moves to
   * the next tick; call it once per tick. An input whose player left the
   * roster is dropped.
   */
  void release(PlayerRoster& roster, const InputSink& sink);

  /** @brief Drops every input held for the player numbered @p playerNumber. */
  void clear(std::uint8_t playerNumber);

 private:
  std::size_t delayTicks_;
  std::uint64_t currentTick_{0};
  std::deque<HeldInput> held_;
};

}  // namespace rtype::server
