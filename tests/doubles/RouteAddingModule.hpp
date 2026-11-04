#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

/**
 * @brief Module double listed after the HTTP module of the master, which adds
 * a route to its server while it installs, as the module of a feature will.
 * The route answers the status 200 with an empty body.
 */
class RouteAddingModule final : public rtype::engine::IModule {
 public:
  /** @param path The route it adds under /api, which is also its name. */
  explicit RouteAddingModule(std::string path);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string path_;
};
