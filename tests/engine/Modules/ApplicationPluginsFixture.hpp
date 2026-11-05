#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "PluginHostState.hpp"
#include "TestService.hpp"

/**
 * @brief What the tests of the plugins share: the services a host module
 * provides to them, the journal of the configuration step and the refusals it
 * collected, and the building of an application whose configuration step runs
 * the lines of the test.
 */
class ApplicationPluginsFixture : public ::testing::Test {
 public:
  ApplicationPluginsFixture() = default;
  ApplicationPluginsFixture(const ApplicationPluginsFixture&) = delete;
  ApplicationPluginsFixture& operator=(const ApplicationPluginsFixture&) =
      delete;
  ApplicationPluginsFixture(ApplicationPluginsFixture&&) = delete;
  ApplicationPluginsFixture& operator=(ApplicationPluginsFixture&&) = delete;
  ~ApplicationPluginsFixture() override = default;

  /** @returns A host module named @p name that provides the services of
   * @p state. */
  [[nodiscard]] std::unique_ptr<rtype::engine::IModule> hostNamed(
      std::string_view name);

  /** @returns A host module named @p name that provides an EventBus and a
   * TestService, and no DeliveredEvents. */
  [[nodiscard]] std::unique_ptr<rtype::engine::IModule> bareHostNamed(
      std::string_view name);

  /**
   * @returns An application of @p host alone, whose configuration step runs
   * `plugin-folder` with the folder the tests build the plugins in, then each
   * of @p lines.
   */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> applicationOf(
      std::unique_ptr<rtype::engine::IModule> host,
      std::vector<std::string> lines);

  /** @brief Publishes one TestEvent on the bus of the host and delivers it. */
  void publishOneEvent();

  PluginHostState state;
  JournalModule journal;
  std::vector<std::string> refusals;
  TestService bareService;
  rtype::engine::EventBus bareBus;
};
