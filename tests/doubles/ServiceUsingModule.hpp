#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "JournalModule.hpp"
#include "ServiceUsingModuleSetup.hpp"

/** @brief Module double that, during its install(), requires the TestService
 * or only looks for it, and writes in a journal whether it got it. It opens
 * nothing. */
class ServiceUsingModule : public rtype::engine::IModule {
 public:
  /** @param journal Where the lookups are written down; it must outlive the
   * module. */
  ServiceUsingModule(ServiceUsingModuleSetup setup, JournalModule& journal);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException, before writing anything, when the setup
   * requires the TestService and no module installed before provides it. */
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  [[nodiscard]] bool foundService(
      const rtype::engine::InstallContext& context) const;
  void record(std::string_view call);

  ServiceUsingModuleSetup setup_;
  JournalModule* journal_;
};
