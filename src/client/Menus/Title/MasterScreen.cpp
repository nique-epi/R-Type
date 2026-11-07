#include "MasterScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include "Endpoint.hpp"
#include "EndpointForm.hpp"
#include "EndpointReading.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "MasterScreenConstants.hpp"
#include "MasterScreenParts.hpp"
#include "MasterUrl.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "NetworkConstants.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The master @p master reads, as its fields show it; for a master
 * named by host, an empty address and network::DEFAULT_MASTER_PORT. */
network::Endpoint shownMasterOf(const IMasterAddress& master) {
  return masterEndpointOf(master.url())
      .value_or(network::Endpoint{.address = {},
                                  .port = network::DEFAULT_MASTER_PORT});
}

}  // namespace

MasterScreen::MasterScreen(MasterScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      settings_(&parts.settings.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      menu_(buildMenu(parts.sounds.get(), shownMasterOf(parts.master.get()))) {
  repaint();
}

void MasterScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void MasterScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  repaint();
}

void MasterScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool MasterScreen::hidesScreensBelow() const { return true; }

Menu MasterScreen::buildMenu(ISoundPlayer& sounds,
                             const network::Endpoint& shown) {
  const std::function<void()> saveAction = [this] { save(); };
  MenuBuilder builder(MenuBuilder::Frame::Form, &sounds);
  builder.heading(MASTER_HEADING);
  return addEndpointFields(builder, shown, saveAction)
      .button(
          {.label = SAVE_MASTER_LABEL, .description = SAVE_MASTER_DESCRIPTION},
          saveAction)
      .cancelButton({.label = BACK_LABEL}, [this] { navigator_->pop(); })
      .build();
}

void MasterScreen::save() {
  const EndpointReading reading = readEndpoint(menu_);
  problem_ = reading.problem;
  if (!reading.endpoint.has_value()) {
    return;
  }
  static_cast<void>(settings_->apply(MASTER_SETTING, reading.endpoint));
  static_cast<void>(settings_->saveIfChanged());
  navigator_->pop();
}

void MasterScreen::repaint() {
  menu_.setStatus(std::string{problem_});
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
