#include "RouletteScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include "Ambience.hpp"
#include "BetsReading.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "ChipRow.hpp"
#include "ChipSelector.hpp"
#include "ChipSelectorConstants.hpp"
#include "ChipsAtHand.hpp"
#include "ClothBets.hpp"
#include "ClothDirection.hpp"
#include "ClothMoves.hpp"
#include "HeldKey.hpp"
#include "ICasinoChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "IWallet.hpp"
#include "InterfacePixel.hpp"
#include "RouletteBarActions.hpp"
#include "RouletteBet.hpp"
#include "RouletteMemory.hpp"
#include "RoulettePainting.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
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

/** @returns Whether @p room takes bets. */
bool betsOpenIn(const std::optional<network::CasinoRoomState>& room) {
  return room.has_value() && room->phase == network::RoulettePhase::Betting;
}

/** @returns The chips the player may lay on the round in progress: those the
 * master left and those it holds on the cloth once it took bets of the round,
 * those of @p wallet before. */
std::uint32_t chipsAtHandIn(const RouletteMemory& memory,
                            const IWallet& wallet) {
  return chipsAtHandOf(walletChips(wallet).value_or(0), memory.accepted());
}

/** @returns Why the master refused @p bets, the newest, on the round of
 * @p room, or else why @p casino refused the seat in it. */
std::optional<CasinoRefusal> refusalOf(
    const ICasinoChannel& casino, const std::optional<BetsReading>& bets,
    const std::optional<network::CasinoRoomState>& room) {
  if (room.has_value() && bets.has_value() && bets->refusal.has_value() &&
      bets->round.round == room->round) {
    return bets->refusal;
  }
  return casino.seatRefusal();
}

}  // namespace

RouletteScreen::RouletteScreen(RouletteScreenParts parts,
                               std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::CasinoTables),
      sounds_(&parts.sounds.get()),
      roundSounds_(parts.sounds.get()),
      casino_(&parts.casino.get()),
      memory_(&parts.memory.get()),
      wallet_(&parts.wallet.get()),
      account_(&parts.account.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      palette_(parts.palette),
      motion_(parts.motion),
      bars_(
          RouletteBarActions{.clear =
                                 [this] {
                                   memory_->slip().clear();
                                   send();
                                 },
                             .rebet =
                                 [this] {
                                   if (memory_->slip().replaceWith(
                                           memory_->previousBets(),
                                           chipsAtHandIn(*memory_, *wallet_))) {
                                     send();
                                   }
                                 },
                             .back = [this] { navigator_->pop(); }},
          parts.sounds) {
  const std::optional<network::CasinoRoomState> room = casino_->room();
  const std::optional<BetsReading>& bets = casino_->lastBets();
  memory_->follow(room, bets);
  const RouletteReading reading = readTable(room, bets);
  roundSounds_.follow(reading);
  repaint(reading);
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

void RouletteScreen::update([[maybe_unused]] engine::Duration elapsed) {
  const std::optional<network::CasinoRoomState> room = casino_->room();
  const std::optional<BetsReading>& bets = casino_->lastBets();
  memory_->follow(room, bets);
  const RouletteReading reading = readTable(room, bets);
  roundSounds_.follow(reading);
  repaint(reading);
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
  const std::optional<network::CasinoRoomState> room = casino_->room();
  memory_->follow(room, casino_->lastBets());
  if (!betsOpenIn(room)) {
    return;
  }
  network::RouletteBet bet = betAt(cursor_);
  bet.amount = CHIP_VALUES.at(chosenChip_);
  if (memory_->slip().lay(bet, chipsAtHandIn(*memory_, *wallet_))) {
    sounds_->play(CLICK_SOUND_ID);
    send();
  }
}

void RouletteScreen::send() {
  if (const std::optional<network::CasinoRoomState> room = casino_->room()) {
    casino_->placeBets(room->round, memory_->slip().bets());
  }
}

RouletteReading RouletteScreen::readTable(
    const std::optional<network::CasinoRoomState>& room,
    const std::optional<BetsReading>& bets) const {
  return RouletteReading{.room = room,
                         .ownNickname = account_->nickname(),
                         .walletChips = walletChips(*wallet_).value_or(0),
                         .bets = memory_->slip().bets(),
                         .accepted = memory_->accepted(),
                         .ownBets = room.has_value()
                                        ? casino_->ownBets(room->round)
                                        : std::nullopt,
                         .refusal = refusalOf(*casino_, bets, room),
                         .previousBets = memory_->previousBets(),
                         .cursor = cursor_,
                         .chosenChip = chosenChip_,
                         .palette = palette_,
                         .motion = motion_};
}

void RouletteScreen::repaint(const RouletteReading& reading) {
  bars_.show(barStatesOf(reading));
  canvas_->clear();
  paintRoulette(*canvas_, rouletteViewOf(reading));
  bars_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
