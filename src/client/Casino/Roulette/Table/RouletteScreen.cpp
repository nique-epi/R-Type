#include "RouletteScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include "ChipRow.hpp"
#include "ChipSelector.hpp"
#include "ChipSelectorConstants.hpp"
#include "ClothBets.hpp"
#include "ClothDirection.hpp"
#include "ClothMoves.hpp"
#include "HeldKey.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "RouletteBarActions.hpp"
#include "RouletteBet.hpp"
#include "RoulettePainting.hpp"
#include "RouletteReading.hpp"
#include "RouletteRound.hpp"
#include "RouletteScene.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteScreenParts.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "WalletPurse.hpp"

namespace rtype::client {

namespace {

/** @returns Where @p key moves the cursor, if it is an arrow. */
std::optional<ClothDirection> directionOfKey(sf::Keyboard::Key key) {
  switch (key) {
    case CURSOR_LEFT_KEY:
      return ClothDirection::Left;
    case CURSOR_RIGHT_KEY:
      return ClothDirection::Right;
    case CURSOR_UP_KEY:
      return ClothDirection::Up;
    case CURSOR_DOWN_KEY:
      return ClothDirection::Down;
    default:
      break;
  }
  return std::nullopt;
}

/** @returns Whether @p round takes bets. */
bool betsOpenIn(const std::optional<network::RouletteRound>& round) {
  return round.has_value() && round->phase == network::RoulettePhase::Betting;
}

}  // namespace

RouletteScreen::RouletteScreen(RouletteScreenParts parts,
                               std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      casino_(&parts.casino.get()),
      memory_(&parts.memory.get()),
      wallet_(&parts.wallet.get()),
      connection_(&parts.connection.get()),
      waitingRoom_(&parts.waitingRoom.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      palette_(parts.palette),
      motion_(parts.motion),
      bars_(
          RouletteBarActions{.spin =
                                 [this] {
                                   memory_->askSpin();
                                   send();
                                 },
                             .clear =
                                 [this] {
                                   memory_->slip().clear();
                                   send();
                                 },
                             .rebet =
                                 [this] {
                                   if (memory_->slip().replaceWith(
                                           memory_->previousBets(),
                                           walletChips(*wallet_).value_or(0))) {
                                     send();
                                   }
                                 },
                             .back = [this] { navigator_->pop(); }},
          parts.sounds) {
  seenRound_ = casino_->round();
  memory_->follow(seenRound_, walletChips(*wallet_).value_or(0));
  repaint();
}

void RouletteScreen::handleEvent(const sf::Event& event) {
  if (bars_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (releasesHeldKey(event, heldKey_)) {
    heldKey_.reset();
  } else if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(pressed->code);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(playfieldPointAt_(click->position)));
  }
}

void RouletteScreen::update(engine::Duration elapsed) {
  std::optional<network::RouletteRound> round = casino_->round();
  memory_->follow(round, walletChips(*wallet_).value_or(0));
  if (round != seenRound_) {
    seenRound_ = std::move(round);
    sinceRound_ = engine::Duration::zero();
  } else {
    sinceRound_ += elapsed;
  }
  repaint();
}

void RouletteScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool RouletteScreen::hidesScreensBelow() const { return true; }

void RouletteScreen::keyPressed(sf::Keyboard::Key key) {
  if (const std::optional<ClothDirection> direction = directionOfKey(key)) {
    cursor_ = placeAfter(cursor_, *direction);
    return;
  }
  if (heldKey_ == key) {
    return;
  }
  heldKey_ = key;
  if (key == LAY_CHIP_KEY) {
    layChip();
  } else if (const std::optional<std::size_t> chip = chipOfKey(key)) {
    chosenChip_ = *chip;
    sounds_->play(CLICK_SOUND_ID);
  }
}

void RouletteScreen::pointerPressed(sf::Vector2i pixel) {
  if (const std::optional<std::size_t> chip =
          chipAt(ChipRow{.firstChip = BAND_FIRST_CHIP, .chosen = chosenChip_},
                 pixel)) {
    chosenChip_ = *chip;
    sounds_->play(CLICK_SOUND_ID);
  }
}

void RouletteScreen::layChip() {
  const std::optional<network::RouletteRound> round = casino_->round();
  memory_->follow(round, walletChips(*wallet_).value_or(0));
  if (!betsOpenIn(round)) {
    return;
  }
  network::RouletteBet bet = betAt(cursor_);
  bet.amount = CHIP_VALUES.at(chosenChip_);
  if (memory_->slip().lay(bet, walletChips(*wallet_).value_or(0))) {
    sounds_->play(CLICK_SOUND_ID);
    send();
  }
}

void RouletteScreen::send() {
  casino_->placeBets(memory_->slip().bets(), memory_->spinAsked());
}

RouletteReading RouletteScreen::readTable() const {
  return RouletteReading{
      .round = seenRound_,
      .timeLeft = seenRound_.has_value()
                      ? phaseTimeLeft(seenRound_->ticksLeft, sinceRound_)
                      : engine::Duration::zero(),
      .ownPlayerNumber = connection_->status().playerNumber,
      .players = waitingRoom_->lobbyPlayers(),
      .chipsHeld = walletChips(*wallet_).value_or(0),
      .chipsBeforeDraw = memory_->chipsBeforeDraw(),
      .bets = memory_->slip().bets(),
      .spinAsked = memory_->spinAsked(),
      .previousBets = memory_->previousBets(),
      .lastNumbers = memory_->lastNumbers(),
      .cursor = cursor_,
      .chosenChip = chosenChip_,
      .palette = palette_,
      .motion = motion_};
}

void RouletteScreen::repaint() {
  const RouletteReading reading = readTable();
  bars_.show(barStatesOf(reading));
  canvas_->clear();
  paintRoulette(*canvas_, rouletteViewOf(reading));
  bars_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
