#include "Application.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationStep.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "RequireNoNullModule.hpp"
#include "TimeConstants.hpp"
#if RTYPE_MODULE_PLUGINS
#include "ExecutablePath.hpp"
#include "PluginConstants.hpp"
#endif

namespace rtype::engine {

namespace {

bool listsModule(std::span<const std::unique_ptr<IModule>> modules,
                 const std::string& name) {
  return std::ranges::any_of(
      modules, [&name](const std::unique_ptr<IModule>& listedModule) {
        return listedModule->name() == name;
      });
}

std::vector<std::string> optionalDependenciesOf(const IModule& module) {
  const auto* const optional =
      dynamic_cast<const IOptionalDependencies*>(&module);
  if (optional == nullptr) {
    return {};
  }
  return optional->optionalDependencies();
}

/** @brief Throws for a dependency of the module at @p position that is not
 * listed before it, or an optional one that is listed after it. */
void requireListedBefore(std::span<const std::unique_ptr<IModule>> modules,
                         std::size_t position) {
  const std::span<const std::unique_ptr<IModule>> listedBefore =
      modules.first(position);
  const IModule& module = *modules[position];
  for (const std::string& dependency : module.dependencies()) {
    if (!listsModule(listedBefore, dependency)) {
      throw MissingDependencyException(module.name(), dependency);
    }
  }
  for (const std::string& dependency : optionalDependenciesOf(module)) {
    if (listsModule(modules, dependency) &&
        !listsModule(listedBefore, dependency)) {
      throw MissingDependencyException(module.name(), dependency);
    }
  }
}

void requireDependenciesListedBefore(
    std::span<const std::unique_ptr<IModule>> modules) {
  for (std::size_t position = 0; position < modules.size(); ++position) {
    requireListedBefore(modules, position);
  }
}

}  // namespace

Application::Application(std::vector<std::unique_ptr<IModule>> modules)
    : Application(LoopStages(std::span<const std::string_view>{}),
                  std::move(modules)) {}

Application::Application(LoopStages stages,
                         std::vector<std::unique_ptr<IModule>> modules,
                         ConfigurationStep configure)
    :
#if RTYPE_MODULE_PLUGINS
      plugins_(executableFolder() / DEFAULT_PLUGIN_FOLDER_NAME),
#endif
      stages_(std::move(stages)),
      modules_(std::move(modules)),
      configure_(std::move(configure)) {
  requireNoNullModule<IModule>(modules_);
  requireDependenciesListedBefore(modules_);
  services_.provide(*this);
  services_.provide(commands_);
  services_.provide(configuration_);
  switches_.registerCommand(commands_);
#if RTYPE_MODULE_PLUGINS
  plugins_.registerCommands(commands_);
#endif
}

Application::~Application() {
  stop();
  while (!modules_.empty()) {
    modules_.pop_back();
  }
}

void Application::start() {
  if (startCalled_) {
    return;
  }
  startCalled_ = true;
  loadPlugins();
  settleSwitches();
  installModules();
  if (configure_) {
    configure_(commands_);
  }
  startModules();
}

void Application::stop() {
  {
    const std::scoped_lock lock{stagesMutex_};
    running_ = false;
  }
  while (!started_.empty()) {
    IModule* const lastStarted = started_.back();
    started_.pop_back();
    lastStarted->stop();
  }
}

void Application::runStages(Duration elapsed) const {
  const std::scoped_lock lock{stagesMutex_};
  if (running_) {
    stages_.run(elapsed);
  }
}

void Application::loadPlugins() {
  if (!configure_) {
    return;
  }
  CommandInterpreter pluginsOnly;
#if RTYPE_MODULE_PLUGINS
  plugins_.registerCommands(pluginsOnly);
#endif
  configure_(pluginsOnly);
#if RTYPE_MODULE_PLUGINS
  addModules(plugins_.takeModules());
#endif
}

void Application::addModules(std::vector<std::unique_ptr<IModule>> added) {
  for (std::unique_ptr<IModule>& addedModule : added) {
    if (listsModule(modules_, addedModule->name())) {
      throw DuplicateModuleNameException(addedModule->name());
    }
    modules_.push_back(std::move(addedModule));
  }
  requireDependenciesListedBefore(modules_);
}

void Application::settleSwitches() {
  if (!configure_) {
    return;
  }
  CommandInterpreter switchesOnly;
  switches_.registerCommand(switchesOnly);
  configure_(switchesOnly);
}

void Application::installModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    if (!switches_.isOn(listedModule->name())) {
      continue;
    }
    InstallContext context(listedModule->name(), services_, stages_);
    listedModule->install(context);
  }
}

void Application::startModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    if (!switches_.isOn(listedModule->name())) {
      continue;
    }
    try {
      listedModule->start();
    } catch (const std::exception& error) {
      stop();
      throw ModuleStartFailedException(listedModule->name(), error.what());
    }
    started_.push_back(listedModule.get());
  }
  const std::scoped_lock lock{stagesMutex_};
  running_ = true;
}

}  // namespace rtype::engine
