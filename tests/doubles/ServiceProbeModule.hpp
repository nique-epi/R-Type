#pragma once

#include <string>
#include <vector>
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "StageWork.hpp"
#include "TickDatagrams.hpp"

/** @brief Module double that looks up, during its install(), the services the
 * network module provides and keeps what it found, and adds the work the test
 * gives it to the simulation stage. It opens nothing. */
class ServiceProbeModule : public rtype::engine::IModule {
 public:
  /** @param dependencies The names of the modules it is listed after. */
  ServiceProbeModule(std::string name, std::vector<std::string> dependencies,
                     rtype::engine::StageWork work);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns The services found, or nullptr for one no module provided. */
  [[nodiscard]] rtype::server::IDatagramSender* sender() const;
  [[nodiscard]] rtype::server::IPlayerAdmission* admission() const;
  [[nodiscard]] rtype::server::TickDatagrams* tickDatagrams() const;

 private:
  std::string name_;
  std::vector<std::string> dependencies_;
  rtype::engine::StageWork work_;
  rtype::server::IDatagramSender* sender_{nullptr};
  rtype::server::IPlayerAdmission* admission_{nullptr};
  rtype::server::TickDatagrams* tickDatagrams_{nullptr};
};
