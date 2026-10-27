#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "JournalModule.hpp"
#include "StageModuleSetup.hpp"

/** @brief Module double that adds, during its install(), one work to a stage;
 * the work writes the stage and the module's name in a journal shared with the
 * other modules of the test. It opens nothing. */
class StageModule : public rtype::engine::IModule {
 public:
  /** @param journal Where the runs are written down; it must outlive the
   * module. */
  StageModule(StageModuleSetup setup, JournalModule& journal);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  void recordRun();

  StageModuleSetup setup_;
  JournalModule* journal_;
};
