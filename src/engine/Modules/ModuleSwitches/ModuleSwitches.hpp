#pragma once

#include <memory>
#include <set>
#include <span>
#include <string>
#include "CommandInterpreter.hpp"
#include "IModule.hpp"

namespace rtype::engine {

/**
 * @brief Which modules of a program are turned off, and the `module` command
 * that turns them on and off.
 *
 * A module that is off is never installed nor started. The command refuses a
 * change that would leave a started module without a module it depends on:
 * turning off a module a started module depends on, and turning on a module
 * whose dependency is off.
 */
class ModuleSwitches {
 public:
  /**
   * @param modules The modules of the program; the list and the modules must
   * outlive the switches.
   */
  explicit ModuleSwitches(std::span<const std::unique_ptr<IModule>> modules);

  ModuleSwitches(const ModuleSwitches&) = delete;
  ModuleSwitches& operator=(const ModuleSwitches&) = delete;
  ModuleSwitches(ModuleSwitches&&) = delete;
  ModuleSwitches& operator=(ModuleSwitches&&) = delete;
  ~ModuleSwitches() = default;

  /**
   * @brief Registers `module <name> on|off` in @p interpreter, which must
   * outlive the switches.
   * @throws DuplicateCommandException if the interpreter already has it.
   */
  void registerCommand(CommandInterpreter& interpreter);

  /** @returns Whether the module called @p name is on; every module is, until
   * the command turns it off. */
  [[nodiscard]] bool isOn(const std::string& name) const;

 private:
  void switchModule(std::span<const std::string> arguments);
  void turnOn(const IModule& module);
  void turnOff(const IModule& module);
  [[nodiscard]] const IModule& listed(const std::string& name) const;
  [[nodiscard]] std::string listedNames() const;

  std::span<const std::unique_ptr<IModule>> modules_;
  std::set<std::string> off_;
};

}  // namespace rtype::engine
