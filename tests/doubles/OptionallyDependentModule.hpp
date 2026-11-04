#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "InstallContext.hpp"

/** @brief A module that depends on nothing and uses another one when it is
 * listed; it does nothing when installed, started or stopped. */
class OptionallyDependentModule : public rtype::engine::IModule,
                                  public rtype::engine::IOptionalDependencies {
 public:
  OptionallyDependentModule(std::string name, std::string optional);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  std::string optional_;
};
