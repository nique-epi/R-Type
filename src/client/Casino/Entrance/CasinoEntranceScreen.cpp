#include "CasinoEntranceScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include "ButlerMotion.hpp"
#include "CasinoBalances.hpp"
#include "CasinoBarChip.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoEntrancePainting.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "EntranceStage.hpp"
#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"
#include "WalletPurse.hpp"

namespace rtype::client {

namespace {

/** @returns Whether one of @p chips took @p event; the others are not told
 * once one did. */
template <typename... Chips>
bool takenByOne(const sf::Event& event, const PlayfieldMapping& pointAt,
                Chips&... chips) {
  return (chips.handleEvent(event, pointAt) || ...);
}

/** @returns Whether @p event skips the opening of the doors: a key pressed,
 * or the left button. */
bool skipsOpening(const sf::Event& event) {
  const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
  return event.is<sf::Event::KeyPressed>() ||
         (click != nullptr && click->button == sf::Mouse::Button::Left);
}

/** @returns The first room of @p rooms with a seat free; nothing before the
 * master listed them, or when every one is full. */
std::optional<std::uint8_t> freeRoomOf(
    const std::optional<network::CasinoRoomListing>& rooms) {
  if (!rooms.has_value()) {
    return std::nullopt;
  }
  const auto free = std::ranges::find_if(
      rooms->rooms, [](const network::CasinoRoom& room) noexcept {
        return room.seated < room.seats;
      });
  if (free == rooms->rooms.end()) {
    return std::nullopt;
  }
  return free->id;
}

}  // namespace

CasinoEntranceScreen::CasinoEntranceScreen(
    CasinoEntranceScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      casino_(&parts.casino.get()),
      wallet_(&parts.wallet.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      motion_(parts.motion),
      enterChip_(chipOf(
          ENTER_BAR, [this] { enter(); }, parts.sounds)),
      doorsBackChip_(chipOf(
          DOORS_BACK_BAR, [this] { navigator_->pop(); }, parts.sounds)),
      rouletteChip_(chipOf(
          ROULETTE_BAR, [this] { playRoulette(); }, parts.sounds)),
      buyChipsChip_(chipOf(
          BUY_CHIPS_BAR, [this] { openOver(CASINO_BUY_CHIPS_SCREEN); },
          parts.sounds)),
      cashOutChip_(chipOf(
          CASH_OUT_BAR, [this] { openOver(CASINO_CASH_OUT_SCREEN); },
          parts.sounds)),
      leaveChip_(chipOf(LEAVE_BAR, [this] { leave(); }, parts.sounds)) {
  casino_->askRooms();
  repaint();
}

void CasinoEntranceScreen::handleEvent(const sf::Event& event) {
  switch (stage_) {
    case EntranceStage::Doors:
      static_cast<void>(
          takenByOne(event, playfieldPointAt_, enterChip_, doorsBackChip_));
      return;
    case EntranceStage::Opening:
      if (skipsOpening(event)) {
        reach(EntranceStage::Hall);
      }
      return;
    case EntranceStage::Hall:
      static_cast<void>(takenByOne(event, playfieldPointAt_, rouletteChip_,
                                   buyChipsChip_, cashOutChip_, leaveChip_));
      return;
  }
}

void CasinoEntranceScreen::update(engine::Duration elapsed) {
  sinceStage_ += elapsed;
  sinceRoomsAsked_ += elapsed;
  if (sinceRoomsAsked_ >= HALL_ROOMS_REFRESH) {
    sinceRoomsAsked_ = engine::Duration::zero();
    casino_->askRooms();
  }
  rouletteChip_.setEnabled(freeRoomOf(casino_->rooms()).has_value());
  if (stage_ == EntranceStage::Opening && sinceStage_ >= OPENING_DURATION) {
    reach(EntranceStage::Hall);
  }
  repaint();
}

void CasinoEntranceScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CasinoEntranceScreen::hidesScreensBelow() const { return true; }

void CasinoEntranceScreen::enter() {
  reach(motion_ == MotionPreference::Full ? EntranceStage::Opening
                                          : EntranceStage::Hall);
}

void CasinoEntranceScreen::leave() {
  casino_->leave();
  navigator_->pop();
}

void CasinoEntranceScreen::playRoulette() {
  if (const std::optional<std::uint8_t> room = freeRoomOf(casino_->rooms())) {
    casino_->enter(*room);
    openOver(ROULETTE_SCREEN);
  }
}

void CasinoEntranceScreen::openOver(ScreenId screen) {
  for (KeyChip* chip :
       {&rouletteChip_, &buyChipsChip_, &cashOutChip_, &leaveChip_}) {
    static_cast<void>(
        chip->handleEvent(sf::Event::FocusLost{}, playfieldPointAt_));
  }
  navigator_->push(screen);
}

void CasinoEntranceScreen::reach(EntranceStage stage) {
  stage_ = stage;
  sinceStage_ = engine::Duration::zero();
}

void CasinoEntranceScreen::repaint() {
  const bool inHall = stage_ == EntranceStage::Hall;
  canvas_->clear();
  paintEntrance(
      *canvas_,
      EntranceView{.front = doorsPictureAt(stage_, sinceStage_),
                   .butler = butlerFrameAt(stage_, sinceStage_, motion_),
                   .butlerLeft = butlerLeftAt(stage_),
                   .line = inHall ? HALL_LINE : WELCOME_LINE,
                   .hall = inHall ? std::optional(CasinoBalances{
                                        .kopecks = walletKopecks(*wallet_),
                                        .chips = walletChips(*wallet_)})
                                  : std::nullopt});
  if (inHall) {
    for (const KeyChip* chip :
         {&rouletteChip_, &buyChipsChip_, &cashOutChip_, &leaveChip_}) {
      chip->paint(*canvas_);
    }
  } else if (stage_ == EntranceStage::Doors) {
    enterChip_.paint(*canvas_);
    doorsBackChip_.paint(*canvas_);
  }
  canvas_->display();
}

}  // namespace rtype::client
