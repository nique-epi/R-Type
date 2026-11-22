#include "ServerMapTabs.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "InterfacePixel.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapTab.hpp"
#include "SoundConstants.hpp"
#include "TabRailTab.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The tab a key opens, nothing for a key of no tab. */
std::optional<std::size_t> tabOfKey(sf::Keyboard::Key key) {
  for (std::size_t tab = 0; tab < SERVER_MAP_TABS.size(); ++tab) {
    if (SERVER_MAP_TABS[tab].key == key) {
      return tab;
    }
  }
  return std::nullopt;
}

/** @returns How the rail shows each tab of the map, in their order, written
 * in @p language. */
std::vector<TabRailTab> railTabsOfTheMap(Language language) {
  std::vector<TabRailTab> tabs;
  tabs.reserve(SERVER_MAP_TABS.size());
  for (const ServerMapTab& entry : SERVER_MAP_TABS) {
    tabs.push_back(TabRailTab{.label = std::string(entry.label.in(language)),
                              .locked = entry.locked});
  }
  return tabs;
}

}  // namespace

ServerMapTabs::ServerMapTabs(IScreenNavigator& navigator, ISoundPlayer& sounds,
                             Language language)
    : navigator_(&navigator),
      sounds_(&sounds),
      rail_(railTabsOfTheMap(language), SECTORS_TAB,
            SERVER_MAP_TABS_TOP_MIDDLE) {}

void ServerMapTabs::handleEvent(const sf::Event& event,
                                const PlayfieldMapping& playfieldPointAt) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(pressed->code);
  } else if (const auto* released = event.getIf<sf::Event::KeyReleased>();
             released != nullptr && released->code == heldKey_) {
    heldKey_.reset();
  } else if (event.is<sf::Event::FocusLost>()) {
    heldKey_.reset();
    rail_.hover(std::nullopt);
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    rail_.hover(
        rail_.tabAt(interfacePixelAt(playfieldPointAt(moved->position))));
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    if (const std::optional<std::size_t> tab =
            rail_.tabAt(interfacePixelAt(playfieldPointAt(click->position)))) {
      choose(*tab);
    }
  }
}

void ServerMapTabs::update(engine::Duration elapsed) { rail_.advance(elapsed); }

void ServerMapTabs::paint(IInterfaceCanvas& canvas) const {
  rail_.paint(canvas);
}

void ServerMapTabs::keyPressed(sf::Keyboard::Key key) {
  const std::optional<std::size_t> tab = tabOfKey(key);
  if (!tab.has_value() || heldKey_ == key) {
    return;
  }
  heldKey_ = key;
  choose(*tab);
}

void ServerMapTabs::choose(std::size_t tab) {
  if (rail_.isLocked(tab)) {
    rail_.refuse(tab);
    return;
  }
  const std::optional<ScreenId> screen = SERVER_MAP_TABS.at(tab).screen;
  if (!screen.has_value()) {
    return;
  }
  heldKey_.reset();
  rail_.hover(std::nullopt);
  sounds_->play(CLICK_SOUND_ID);
  navigator_->push(*screen);
}

}  // namespace rtype::client
