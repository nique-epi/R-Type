#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "IModule.hpp"

namespace rtype::engine {

/**
 * @brief Installs, starts and stops the modules of a program in a known order.
 *
 * The order is the one of the list given to the constructor. start() installs
 * every module in that order, then starts every module in that order; stop()
 * stops the started modules in the reverse order. A module comes after every
 * module it depends on, so its dependencies are installed and started before
 * it, and stopped after it.
 */
class Application {
 public:
  /**
   * @brief Takes the modules and checks the list, before any module is
   * installed.
   *
   * @param modules The modules of the program, in the order they install and
   * start.
   * @throws NullModuleException if a module is null.
   * @throws MissingDependencyException if a dependency of a module is not
   * listed before it.
   */
  explicit Application(std::vector<std::unique_ptr<IModule>> modules);

  /** @brief Stops the modules still started, as stop() does. */
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

  /**
   * @brief Installs every module, then starts every module, in list order.
   *
   * Only the first call does anything: a later call, even after stop(),
   * installs and starts nothing. When a module throws from start(), the modules
   * started before it are stopped, the last started first, then the error is
   * thrown; the module that threw is not stopped. An exception thrown by
   * install() goes through unchanged, and no module is started.
   *
   * @throws ModuleStartFailedException if a module throws from start(); its
   * message names the module and gives the reason.
   */
  void start();

  /**
   * @brief Stops the started modules, the last started first.
   *
   * A module is stopped once: a second call stops nothing.
   */
  void stop();

 private:
  void installModules();
  void startModules();

  std::vector<std::unique_ptr<IModule>> modules_;
  std::size_t startedCount_{0};
  bool startCalled_{false};
};

}  // namespace rtype::engine
