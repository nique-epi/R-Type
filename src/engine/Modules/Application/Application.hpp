#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief Installs, starts and stops the modules of a program in a known order,
 * and runs the work they added to the stages of the program's loop.
 *
 * The order is the one of the list given to the constructor. start() installs
 * every module in that order, then starts every module in that order; stop()
 * stops the started modules in the reverse order. A module comes after every
 * module it depends on, so its dependencies are installed and started before
 * it, and stopped after it. A module finds, during its install(), the services
 * of the modules installed before it, and the application itself: a module
 * that drives the loop (the one that owns the thread a program ticks on) asks
 * for the Application to call runStages().
 */
class Application {
 public:
  /**
   * @brief Builds an application whose loop has no stage, as a program that
   * runs no work through it does.
   *
   * @throws NullModuleException if a module is null.
   * @throws MissingDependencyException if a dependency of a module is not
   * listed before it.
   */
  explicit Application(std::vector<std::unique_ptr<IModule>> modules);

  /**
   * @brief Takes the stages of the loop and the modules, and checks the list,
   * before any module is installed.
   *
   * @param stages The stages of the program's loop, in the order they run,
   * still without work: the modules add theirs while they install.
   * @param modules The modules of the program, in the order they install and
   * start.
   * @throws NullModuleException if a module is null.
   * @throws MissingDependencyException if a dependency of a module is not
   * listed before it.
   */
  Application(LoopStages stages, std::vector<std::unique_ptr<IModule>> modules);

  /**
   * @brief Stops the modules still started, as stop() does, then destroys the
   * modules, the last listed first, so a module is destroyed before the
   * modules whose services it holds.
   */
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
   * install() goes through unchanged, no later module is installed and no
   * module is started.
   *
   * @throws ModuleStartFailedException if a module throws from start(); its
   * message names the module and gives the reason.
   * @throws MissingServiceException if a module requires a service no module
   * installed before it provides.
   */
  void start();

  /**
   * @brief Stops the started modules, the last started first.
   *
   * The stages stop running first: a pass of runStages() on another thread is
   * waited for, and every later pass runs nothing, so the modules are stopped
   * while no work of theirs runs. A module is stopped once: a second call
   * stops nothing. Call it from a thread that is not running a stage.
   */
  void stop();

  /**
   * @brief Runs every stage of the loop in declared order, each work with
   * @p elapsed, while every module is started; runs nothing before start()
   * returned, after a failed start() and after stop().
   *
   * It may be called from the thread of the loop while another thread calls
   * stop(): stop() waits for the pass in flight.
   */
  void runStages(Duration elapsed) const;

 private:
  void installModules();
  void startModules();

  LoopStages stages_;
  ServiceRegistry services_;
  std::vector<std::unique_ptr<IModule>> modules_;
  std::size_t startedCount_{0};
  bool startCalled_{false};
  /** Held while a pass of runStages() runs, and while running_ changes. */
  mutable std::mutex stagesMutex_;
  bool running_{false};
};

}  // namespace rtype::engine
