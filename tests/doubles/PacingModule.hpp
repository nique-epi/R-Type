#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "IStageRunner.hpp"
#include "InstallContext.hpp"
#include "TimeConstants.hpp"

/**
 * @brief Module double that paces the loop as the network module of the
 * server does: it requires the IStageRunner during its install(), and runs one
 * pass of the loop each time the test asks. It opens nothing.
 */
class PacingModule : public rtype::engine::IModule {
 public:
  /** @returns PACING_MODULE. */
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @brief Runs the stages once with @p elapsed, through the IStageRunner
   * found during install(). */
  void runOnePass(rtype::engine::Duration elapsed) const;

 private:
  const rtype::engine::IStageRunner* stages_{nullptr};
};
