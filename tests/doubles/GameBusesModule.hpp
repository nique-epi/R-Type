#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "IGameBuses.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that stands in for the games module as the
 * IGameBuses only: it holds the bus of each game a test drives by hand, and
 * opens nothing. */
class GameBusesModule : public rtype::engine::IModule,
                        public rtype::server::IGameBuses {
 public:
  /** @param gameCount How many games, so how many buses. */
  explicit GameBusesModule(std::size_t gameCount);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;
  [[nodiscard]] std::span<const std::unique_ptr<rtype::engine::EventBus>>
  buses() const override;

 private:
  std::vector<std::unique_ptr<rtype::engine::EventBus>> buses_;
};
