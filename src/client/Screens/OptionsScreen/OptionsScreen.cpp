#include "OptionsScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "Dropdown.hpp"
#include "IScreenNavigator.hpp"
#include "ISizableWindow.hpp"
#include "Menu.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreenParts.hpp"
#include "OptionsTabs.hpp"
#include "OptionsTabsParts.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"
#include "WindowLayout.hpp"
#include "WindowSizeChart.hpp"

namespace rtype::client {

OptionsScreen::OptionsScreen(OptionsScreenParts parts,
                             std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      place_(parts.place),
      service_(&parts.settings.get()),
      window_(&parts.window.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      dialog_(parts.sounds.get()),
      tabs_(OptionsTabsParts{
          .service = parts.settings,
          .window = parts.window,
          .sounds = parts.sounds,
          .place = parts.place,
          .keyName = std::move(parts.keyName),
          .openTab = [this](std::size_t tab) { tabs_.openTab(tab); },
          .close = [this] { close(); },
          .askToReset =
              [this](const std::string& question,
                     const std::function<void()>& reset) {
                dialog_.ask(question, reset);
              }}) {
  refresh();
  repaint();
}

void OptionsScreen::handleEvent(const sf::Event& event) {
  if (dialog_.isOpen()) {
    dialog_.handleEvent(event, playfieldPointAt_);
    return;
  }
  tabs_.openMenu().handleEvent(event, playfieldPointAt_);
}

void OptionsScreen::update(engine::Duration elapsed) {
  if (dialog_.isOpen()) {
    dialog_.advance(elapsed);
  } else {
    tabs_.openMenu().advance(elapsed);
  }
  stamp_.age(elapsed);
  save();
  refresh();
  repaint();
}

void OptionsScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool OptionsScreen::hidesScreensBelow() const {
  return place_ == OptionsPlace::OverTitle;
}

void OptionsScreen::save() {
  const std::optional<bool> saved = service_->saveIfChanged();
  if (!saved.has_value()) {
    return;
  }
  if (*saved) {
    stamp_.markSaved();
  } else {
    stamp_.markNotSaved();
  }
}

void OptionsScreen::close() {
  save();
  navigator_->pop();
}

void OptionsScreen::refresh() {
  tabs_.refreshDescriptions();
  tabs_.openMenu().setStatus(stamp_.text());
}

void OptionsScreen::repaint() {
  canvas_->clear();
  const Menu& menu = tabs_.openMenu();
  menu.paint(*canvas_);
  const std::optional<sf::IntRect> chart = menu.illustrationArea();
  const WindowLayout& layout = window_->windowLayout();
  if (chart.has_value() && !layout.imposedSize().has_value()) {
    paintWindowSizeChart(*canvas_, chart->position, layout.sizes(),
                         tabs_.windowSizes().highlighted());
  }
  dialog_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
