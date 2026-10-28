#pragma once

#include <string>
#include <vector>
#include "EventBus.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "TestService.hpp"

/** @brief Module double that provides, during its install(), a TestService and
 * an EventBus the test owns. It opens nothing. */
class ProvidingModule : public rtype::engine::IModule {
 public:
  /** @param service, bus What the module provides; both must outlive it. */
  ProvidingModule(std::string name, TestService& service,
                  rtype::engine::EventBus& bus);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  TestService* service_;
  rtype::engine::EventBus* bus_;
};
