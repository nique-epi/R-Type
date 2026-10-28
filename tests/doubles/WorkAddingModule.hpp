#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "StageWork.hpp"

/** @brief Module double that adds, during its install(), the work the test
 * gives it to a stage. It opens nothing. */
class WorkAddingModule : public rtype::engine::IModule {
 public:
  WorkAddingModule(std::string name, std::string stage,
                   rtype::engine::StageWork work);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  std::string stage_;
  rtype::engine::StageWork work_;
};
