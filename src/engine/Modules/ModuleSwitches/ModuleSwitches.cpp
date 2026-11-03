#include "ModuleSwitches.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "ModuleSwitchesConstants.hpp"
#include "NameList.hpp"

namespace rtype::engine {

namespace {

constexpr std::size_t SWITCH_ARGUMENT_COUNT = 2;

bool dependsOn(const IModule& module, const std::string& name) {
  const std::vector<std::string> dependencies = module.dependencies();
  return std::ranges::find(dependencies, name) != dependencies.end();
}

}  // namespace

ModuleSwitches::ModuleSwitches(
    std::span<const std::unique_ptr<IModule>> modules)
    : modules_(modules) {}

void ModuleSwitches::registerCommand(CommandInterpreter& interpreter) {
  interpreter.add(
      CommandDefinition{.name = std::string(MODULE_COMMAND_NAME),
                        .arguments = "<name> on|off",
                        .help = "Turns a module on or off before it starts"},
      [this](std::span<const std::string> arguments) {
        switchModule(arguments);
      });
}

bool ModuleSwitches::isOn(const std::string& name) const {
  return !off_.contains(name);
}

void ModuleSwitches::switchModule(std::span<const std::string> arguments) {
  if (arguments.size() != SWITCH_ARGUMENT_COUNT) {
    throw CommandRefusedException("expected: module <name> on|off");
  }
  const IModule& module = listed(arguments[0]);
  if (arguments[1] == SWITCH_ON_WORD) {
    turnOn(module);
  } else if (arguments[1] == SWITCH_OFF_WORD) {
    turnOff(module);
  } else {
    throw CommandRefusedException("expected 'on' or 'off', got '" +
                                  arguments[1] + "'");
  }
}

void ModuleSwitches::turnOn(const IModule& module) {
  for (const std::string& dependency : module.dependencies()) {
    if (!isOn(dependency)) {
      throw CommandRefusedException("module '" + module.name() +
                                    "' needs module '" + dependency +
                                    "', which is off");
    }
  }
  off_.erase(module.name());
}

void ModuleSwitches::turnOff(const IModule& module) {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    if (isOn(listedModule->name()) && dependsOn(*listedModule, module.name())) {
      throw CommandRefusedException("module '" + listedModule->name() +
                                    "' needs module '" + module.name() +
                                    "', which would be off");
    }
  }
  off_.insert(module.name());
}

const IModule& ModuleSwitches::listed(const std::string& name) const {
  const auto found = std::ranges::find_if(
      modules_, [&name](const std::unique_ptr<IModule>& listedModule) {
        return listedModule->name() == name;
      });
  if (found == modules_.end()) {
    throw CommandRefusedException("no module named '" + name +
                                  "'; modules: " + listedNames());
  }
  return **found;
}

std::string ModuleSwitches::listedNames() const {
  std::vector<std::string> names;
  names.reserve(modules_.size());
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    names.push_back(listedModule->name());
  }
  return joinedNames(names);
}

}  // namespace rtype::engine
