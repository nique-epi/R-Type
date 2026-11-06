#pragma once

#include <string>
#include <vector>
#include "EventBus.hpp"
#include "IGameEvents.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that stands in for the games module as the
 * IGameEvents only: it holds the bus of the game a test drives by hand, and
 * opens nothing. */
class GameEventsModule : public rtype::engine::IModule,
                         public rtype::server::IGameEvents {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;
  [[nodiscard]] rtype::engine::EventBus& events() override;

 private:
  rtype::engine::EventBus events_;
};
