#pragma once

#include <cstddef>
#include "Ambience.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PurchaseRefusal.hpp"
#include "SpokenLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the Bydo of the shop says, in his bubble over his head, with
 * the voice of BYDO_VOICE_SOUND_IDS: a greeting when the shop opens, then
 * where the radio is; the line of each ware bought, and of each refusal; a
 * line when the player reaches for his bottles; a line when the radio is
 * tuned. Each new line cuts the one he was saying. The lines of one kind
 * that come in turn start from the first each time the shop opens.
 */
class ShopkeeperSpeech {
 public:
  /** @param sounds Plays his voice; it must outlive him. */
  ShopkeeperSpeech(ISoundPlayer& sounds, MotionPreference motion);

  /** @brief Greets the player, with the greeting @p ownedCount, how many
   * items the player owns, picks, then says where the radio is. */
  void greet(std::size_t ownedCount);
  /** @brief Says the line of @p item, a position in SHIP_LOOK_ITEM_NAMES. */
  void bought(std::size_t item);
  /** @brief Says why the shop refuses a ware. */
  void refused(PurchaseRefusal refusal);
  /** @brief Says the next of his lines on his bottles. */
  void bottlesTouched();
  /** @brief Says what he thinks of the music the radio now plays,
   * @p playing. */
  void radioTuned(Ambience playing);
  void advance(engine::Duration elapsed);
  /** @brief Paints his bubble, if he speaks, over the booth. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  SpokenLine line_;
  std::size_t tooExpensiveTurn_{};
  std::size_t bottlesTurn_{};
  std::size_t borderRadioTurn_{};
};

}  // namespace rtype::client
