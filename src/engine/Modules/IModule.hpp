#pragma once

#include <string>
#include <vector>
#include "InstallContext.hpp"

namespace rtype::engine {

/**
 * @brief One part of a program that an Application installs, starts and stops.
 *
 * install() prepares the module and keeps nothing open; start() opens what
 * must live until stop() (a socket, a thread, a file kept open, an event
 * subscription); stop() closes it. An Application calls install() once,
 * start() once after every module is installed, and stop() once after a
 * start() that returned.
 */
class IModule {
 public:
  virtual ~IModule() = default;

  /** @returns The name other modules give to depend on this one. */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * @returns The names of the modules this one depends on. Each must come
   * before it in the list given to the Application.
   */
  [[nodiscard]] virtual std::vector<std::string> dependencies() const = 0;

  /**
   * @brief Prepares the module, and keeps nothing open when it returns.
   *
   * It may read a file and close it, build its own objects and check its
   * configuration. Through @p context it provides its services, finds or
   * requires the services of the modules installed before it, and adds its
   * work to the stages of the loop. @p context lives for this call only: the
   * module keeps the services it got, never the context, and hands them to its
   * classes through their constructors.
   *
   * It binds no socket, starts no thread, keeps no file open and subscribes to
   * no event: a module installed but never started is never stopped. A module
   * that subscribes in start() keeps the SubscriptionHandle and unsubscribes in
   * stop().
   *
   * @throws MissingServiceException if it requires a service no module
   * installed before it provides, which stops the installation.
   */
  virtual void install(InstallContext& context) = 0;

  /**
   * @brief Opens what the module needs until stop().
   *
   * A start() that throws first closes what it already opened: the Application
   * does not call stop() on the module whose start() threw.
   *
   * @throws std::exception, or a class derived from it, when the module cannot
   * start.
   */
  virtual void start() = 0;

  /**
   * @brief Closes what start() opened.
   *
   * Must not throw: the Application calls it while it undoes a failed start,
   * where a second error would hide the first, and from its destructor, where
   * an error ends the program.
   */
  virtual void stop() = 0;
};

}  // namespace rtype::engine
