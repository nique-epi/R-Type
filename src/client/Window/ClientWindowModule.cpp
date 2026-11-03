#include "ClientWindowModule.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "EventListeners.hpp"
#include "FrameLoop.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientWindowModule::ClientWindowModule(FrameLoop& loop) : loop_(&loop) {}

std::string ClientWindowModule::name() const {
  return std::string{WINDOW_MODULE_NAME};
}

std::vector<std::string> ClientWindowModule::dependencies() const {
  return {std::string{SETTINGS_MODULE_NAME}};
}

void ClientWindowModule::install(engine::InstallContext& context) {
  settings_ = &context.require<PlayerSettings>();
  context.provide(window_);
  context.provide(listeners_);
  context.provide(*loop_);
  context.addWork(
      EVENT_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { handleEvents(); });
  context.addWork(RENDER_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    window_.renderTarget().clear();
                  });
  context.addWork(
      DISPLAY_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { window_.display(); });
}

void ClientWindowModule::start() {
  window_.open(settings_->windowSize, settings_->displayMode);
}

void ClientWindowModule::stop() { window_.close(); }

void ClientWindowModule::handleEvents() {
  while (const std::optional<sf::Event> event = window_.pollScreenEvent()) {
    listeners_.offer(*event);
  }
  if (!window_.isOpen()) {
    loop_->stop();
  }
}

}  // namespace rtype::client
