#include "Application.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "IModule.hpp"

namespace rtype::engine {

namespace {

void requireNoNullModule(std::span<const std::unique_ptr<IModule>> modules) {
  for (const std::unique_ptr<IModule>& listedModule : modules) {
    if (listedModule == nullptr) {
      throw NullModuleException();
    }
  }
}

bool listsModule(std::span<const std::unique_ptr<IModule>> modules,
                 const std::string& name) {
  return std::ranges::any_of(
      modules, [&name](const std::unique_ptr<IModule>& listedModule) {
        return listedModule->name() == name;
      });
}

void requireDependenciesListedBefore(
    std::span<const std::unique_ptr<IModule>> modules) {
  for (std::size_t position = 0; position < modules.size(); ++position) {
    const std::span<const std::unique_ptr<IModule>> listedBefore =
        modules.first(position);
    for (const std::string& dependency : modules[position]->dependencies()) {
      if (!listsModule(listedBefore, dependency)) {
        throw MissingDependencyException(modules[position]->name(), dependency);
      }
    }
  }
}

}  // namespace

Application::Application(std::vector<std::unique_ptr<IModule>> modules)
    : modules_(std::move(modules)) {
  requireNoNullModule(modules_);
  requireDependenciesListedBefore(modules_);
}

Application::~Application() { stop(); }

void Application::start() {
  if (startCalled_) {
    return;
  }
  startCalled_ = true;
  installModules();
  startModules();
}

void Application::stop() {
  while (startedCount_ > 0) {
    --startedCount_;
    modules_[startedCount_]->stop();
  }
}

void Application::installModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    listedModule->install();
  }
}

void Application::startModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    try {
      listedModule->start();
    } catch (const std::exception& error) {
      stop();
      throw ModuleStartFailedException(listedModule->name(), error.what());
    }
    ++startedCount_;
  }
}

}  // namespace rtype::engine
