#include "MasterScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "MasterScreen.hpp"
#include "MasterScreenParts.hpp"
#include "MasterScreenTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenInputs.hpp"
#include "WidgetConstants.hpp"

using rtype::client::MasterScreen;
using rtype::client::MasterScreenParts;
using rtype::client::PlayerSettings;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::FORM_MASTER_URL;
using rtype::client::testing::HOST_MASTER_URL;

MasterScreenFixture::MasterScreenFixture()
    : MasterScreenFixture(FORM_MASTER_URL) {}

MasterScreenFixture::MasterScreenFixture(std::string_view url)
    : service_(settings_, storage_),
      master_(std::string{url}),
      screen_(MasterScreenParts{.navigator = navigator_,
                                .sounds = sounds_,
                                .settings = service_,
                                .master = master_,
                                .playfieldPointAt = samePlayfieldPoint},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

MasterScreenForAHostFixture::MasterScreenForAHostFixture()
    : MasterScreenFixture(HOST_MASTER_URL) {}

const RecordingNavigator& MasterScreenFixture::navigator() const {
  return navigator_;
}

const PlayerSettings& MasterScreenFixture::settings() const {
  return settings_;
}

const std::vector<PlayerSettings>& MasterScreenFixture::saves() const {
  return storage_.saves();
}

const InterfaceCanvasRecord& MasterScreenFixture::canvas() const {
  return canvas_;
}

void MasterScreenFixture::tap(sf::Keyboard::Key key) { tapKey(screen_, key); }

void MasterScreenFixture::choose(sf::Keyboard::Key key) {
  tap(key);
  screen_.update(PRESS_DURATION);
}

void MasterScreenFixture::type(std::u32string_view text) {
  typeText(screen_, text);
}

void MasterScreenFixture::erase(std::size_t characters) {
  eraseCharacters(screen_, characters);
}
