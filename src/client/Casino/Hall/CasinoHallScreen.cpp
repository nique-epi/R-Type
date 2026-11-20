#include "CasinoHallScreen.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include "Ambience.hpp"
#include "CasinoBarChip.hpp"
#include "CasinoHallConstants.hpp"
#include "CasinoHallScreenParts.hpp"
#include "CasinoLighting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "HallExit.hpp"
#include "HallMoment.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "IntroTimeline.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether one of @p chips took @p event; the others are not told
 * once one did. */
template <typename... Chips>
bool takenByOne(const sf::Event& event, const PlayfieldMapping& pointAt,
                Chips&... chips) {
  return (chips.handleEvent(event, pointAt) || ...);
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

/** @returns The room whose arch lies under @p pixel of the interface. */
std::optional<HallExit> archAt(sf::Vector2i pixel) {
  if (HALL_LEFT_ARCH.contains(pixel)) {
    return HallExit::Cashier;
  }
  if (HALL_RIGHT_ARCH.contains(pixel)) {
    return HallExit::Roulette;
  }
  return std::nullopt;
}

/** @returns The night the hall fades to, @p since the player started to walk
 * out of it: clear at first, whole after HALL_FADE_TIME. */
sf::Color fadeAt(engine::Duration since) {
  const float share =
      std::min(1.0F, secondsOf(since) / secondsOf(HALL_FADE_TIME));
  sf::Color night = CASINO_DEEP_NAVY;
  night.a =
      static_cast<std::uint8_t>(share * static_cast<float>(CASINO_DEEP_NAVY.a));
  return night;
}

}  // namespace

CasinoHallScreen::CasinoHallScreen(CasinoHallScreenParts parts,
                                   std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::CasinoHall),
      casino_(&parts.casino.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      motion_(parts.motion),
      painter_(std::move(parts.picture)),
      cashierChip_(chipOf(
          HALL_CASHIER_BAR, [this] { walkTo(HallExit::Cashier); },
          parts.sounds)),
      rouletteChip_(chipOf(
          HALL_ROULETTE_BAR, [this] { walkTo(HallExit::Roulette); },
          parts.sounds)),
      backChip_(chipOf(HALL_BACK_BAR, [this] { leave(); }, parts.sounds)) {
  casino_->askRooms();
  repaint();
}

void CasinoHallScreen::handleEvent(const sf::Event& event) {
  if (leavingFor_.has_value() || clickOnArch(event)) {
    return;
  }
  static_cast<void>(takenByOne(event, playfieldPointAt_, cashierChip_,
                               rouletteChip_, backChip_));
}

void CasinoHallScreen::update(engine::Duration elapsed) {
  since_ += elapsed;
  sinceRoomsAsked_ += elapsed;
  if (sinceRoomsAsked_ >= HALL_ROOMS_REFRESH) {
    sinceRoomsAsked_ = engine::Duration::zero();
    casino_->askRooms();
  }
  rouletteChip_.setEnabled(freeRoomOf(casino_->rooms()).has_value());
  if (leavingFor_.has_value()) {
    sinceLeaving_ += elapsed;
    if (sinceLeaving_ >= HALL_FADE_TIME) {
      openChosenRoom();
    }
  }
  repaint();
}

void CasinoHallScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CasinoHallScreen::hidesScreensBelow() const { return true; }

void CasinoHallScreen::walkTo(HallExit exit) {
  if (exit == HallExit::Roulette && !freeRoomOf(casino_->rooms()).has_value()) {
    return;
  }
  leavingFor_ = exit;
  sinceLeaving_ = engine::Duration::zero();
  if (motion_ == MotionPreference::Reduced) {
    openChosenRoom();
  }
}

void CasinoHallScreen::openChosenRoom() {
  const std::optional<HallExit> exit = std::exchange(leavingFor_, std::nullopt);
  for (KeyChip* chip : {&cashierChip_, &rouletteChip_, &backChip_}) {
    static_cast<void>(
        chip->handleEvent(sf::Event::FocusLost{}, playfieldPointAt_));
  }
  if (exit == HallExit::Cashier) {
    navigator_->push(CASINO_CASHIER_SCREEN);
    return;
  }
  if (const std::optional<std::uint8_t> room = freeRoomOf(casino_->rooms())) {
    casino_->enter(*room);
    navigator_->push(ROULETTE_SCREEN);
  }
}

void CasinoHallScreen::leave() {
  casino_->leave();
  navigator_->pop();
}

bool CasinoHallScreen::clickOnArch(const sf::Event& event) {
  if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>();
      pressed != nullptr && pressed->button == sf::Mouse::Button::Left) {
    pressedArch_ =
        archAt(interfacePixelAt(playfieldPointAt_(pressed->position)));
    return pressedArch_.has_value();
  }
  const auto* released = event.getIf<sf::Event::MouseButtonReleased>();
  if (released == nullptr || released->button != sf::Mouse::Button::Left ||
      !pressedArch_.has_value()) {
    return false;
  }
  const HallExit pressed = *std::exchange(pressedArch_, std::nullopt);
  if (archAt(interfacePixelAt(playfieldPointAt_(released->position))) ==
      pressed) {
    walkTo(pressed);
  }
  return true;
}

void CasinoHallScreen::repaint() {
  canvas_->clear();
  painter_.paint(*canvas_, HallMoment{.since = since_, .motion = motion_});
  cashierChip_.paint(*canvas_);
  rouletteChip_.paint(*canvas_);
  if (leavingFor_.has_value()) {
    canvas_->fill(INTERFACE_AREA, fadeAt(sinceLeaving_));
  }
  canvas_->display();
}

}  // namespace rtype::client
