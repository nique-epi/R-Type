#pragma once

#include <string>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that requires, during its install(), the Application
 * that installs it, and keeps the address it was given. It opens nothing. */
class ApplicationFindingModule : public rtype::engine::IModule {
 public:
  /** @param found Where the module writes the Application it requires; it
   * must outlive the module. */
  ApplicationFindingModule(std::string name,
                           const rtype::engine::Application*& found);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  const rtype::engine::Application** found_;
};
