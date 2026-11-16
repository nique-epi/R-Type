#include "ShopScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include "FakeWallet.hpp"
#include "KeyPress.hpp"
#include "LookItems.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "ShipLocker.hpp"
#include "ShopScreen.hpp"
#include "ShopScreenParts.hpp"
#include "ShopScreenTestConstants.hpp"
#include "TimeConstants.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"

namespace rtype::client::testing {

ShopScreenFixture::ShopScreenFixture()
    : audio_(sounds_),
      screen_(std::make_unique<ShopScreen>(
          ShopScreenParts{.navigator = navigator_,
                          .sounds = sounds_,
                          .audio = audio_,
                          .wallet = wallet_,
                          .playfieldPointAt = samePlayfieldPoint,
                          .motion = MotionPreference::Reduced},
          std::make_unique<RecordingInterfaceCanvas>(painting_))) {
  wallet_.balances = network::Wallet{};
  wallet_.looks = ShipLocker{};
}

void ShopScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::pointAt(sf::Vector2i pixel, bool clicked) {
  const sf::Vector2i windowPixel = pixel * INTERFACE_PIXEL;
  if (clicked) {
    screen_->handleEvent(leftPress(windowPixel));
    screen_->handleEvent(leftRelease(windowPixel));
  } else {
    screen_->handleEvent(pointerMove(windowPixel));
  }
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::buyChosen() {
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);
  if (!wallet_.boughtItems.empty() && wallet_.balances.has_value() &&
      wallet_.looks.has_value()) {
    const std::size_t item = wallet_.boughtItems.back();
    wallet_.balances->kopecks -= network::lookItemPrice(item);
    wallet_.looks->items.set(item);
    wallet_.purchaseAnswer =
        WalletChangeReading{.ask = wallet_.boughtItems.size(),
                            .wallet = wallet_.balances,
                            .refusal = std::nullopt};
  }
  wait(THE_STAMP_TIME);
}

void ShopScreenFixture::wait(engine::Duration elapsed) {
  screen_->update(elapsed);
}

void ShopScreenFixture::give(std::uint32_t kopecks) {
  wallet_.balances = network::Wallet{.kopecks = kopecks, .chips = 0};
  screen_->update(ONE_FRAME);
}

FakeWallet& ShopScreenFixture::wallet() { return wallet_; }

const RecordingNavigator& ShopScreenFixture::navigator() const {
  return navigator_;
}

const RecordingSoundPlayer& ShopScreenFixture::sounds() const {
  return sounds_;
}

const InterfaceCanvasRecord& ShopScreenFixture::painting() const {
  return painting_;
}

bool showsATicket(const InterfaceCanvasRecord& painting) {
  return std::ranges::any_of(painting.images,
                             [](const RecordedImage& image) noexcept {
                               return image.assetId == TICKET_SHEET;
                             });
}

}  // namespace rtype::client::testing
