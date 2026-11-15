#include "ShopkeeperSpeech.hpp"
#include <cstddef>
#include <span>
#include <string_view>
#include "Ambience.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PurchaseRefusal.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShopkeeperLineConstants.hpp"
#include "ShopkeeperSpeechConstants.hpp"
#include "SoundConstants.hpp"
#include "SpokenLineParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The line of @p lines at @p turn, the first again after the last,
 * and moves the turn on. */
std::string_view inTurn(std::span<const std::string_view> lines,
                        std::size_t& turn) {
  const std::string_view line = lines[turn % lines.size()];
  ++turn;
  return line;
}

}  // namespace

ShopkeeperSpeech::ShopkeeperSpeech(ISoundPlayer& sounds,
                                   MotionPreference motion)
    : line_(SpokenLineParts{.sounds = sounds,
                            .voice = BYDO_VOICE_SOUND_IDS,
                            .width = SHOPKEEPER_BUBBLE_WIDTH,
                            .tail = SHOPKEEPER_BUBBLE_TAIL,
                            .motion = motion}) {}

void ShopkeeperSpeech::greet(std::size_t ownedCount) {
  line_.say(SHOPKEEPER_GREETINGS.at(ownedCount % SHOPKEEPER_GREETINGS.size()));
  line_.sayNext(SHOPKEEPER_RADIO_HINT);
}

void ShopkeeperSpeech::bought(std::size_t item) {
  line_.say(SHOPKEEPER_BOUGHT_LINES.at(item));
}

void ShopkeeperSpeech::refused(PurchaseRefusal refusal, std::size_t item) {
  switch (refusal) {
    case PurchaseRefusal::AlreadyOwned:
      line_.say(SHOPKEEPER_ALREADY_YOURS_LINE);
      return;
    case PurchaseRefusal::NotForSale:
      line_.say(SHOPKEEPER_NOT_FOR_SALE_LINE);
      return;
    case PurchaseRefusal::TooExpensive:
      line_.say(
          identifiers::isHullItem(item)
              ? SHOPKEEPER_HULL_TOO_EXPENSIVE_LINES.at(
                    item - identifiers::SHIP_LOOK_ACCESSORY_COUNT)
              : inTurn(SHOPKEEPER_TOO_EXPENSIVE_LINES, tooExpensiveTurn_));
      return;
  }
}

void ShopkeeperSpeech::bottlesTouched() {
  line_.say(inTurn(SHOPKEEPER_BOTTLES_LINES, bottlesTurn_));
}

void ShopkeeperSpeech::radioTuned(Ambience playing) {
  line_.say(playing == Ambience::ShopRadio
                ? inTurn(SHOPKEEPER_BORDER_RADIO_LINES, borderRadioTurn_)
                : SHOPKEEPER_SHOP_MUSIC_LINE);
}

void ShopkeeperSpeech::advance(engine::Duration elapsed) {
  line_.advance(elapsed);
}

void ShopkeeperSpeech::paint(IInterfaceCanvas& canvas) const {
  line_.paint(canvas, SHOPKEEPER_BUBBLE_BOTTOM_LEFT);
}

}  // namespace rtype::client
