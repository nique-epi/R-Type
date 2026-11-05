#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "EventBus.hpp"
#include "GameBusesModule.hpp"
#include "PluginHostState.hpp"
#include "RecordingJournal.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief The base of the tests of the game statistics plugin: an application
 * of a logger that records, a network that gives a port and the buses of two
 * games the test drives, whose configuration loads the compiled plugin.
 */
class ServerGameStatisticsFixture : public ::testing::Test {
 protected:
  /** @brief Starts the application, the plugin writing to
   * STATISTICS_FILE_NAME in the temporary folder. */
  void start();

  /** @brief Starts the application, the plugin writing to @p file.
   * @throws what Application::start() throws. */
  void startWritingTo(const std::filesystem::path& file);

  /** @brief Starts the application with no `game-statistics-file` line. */
  void startWithTheDefaultFile();

  /** @brief Publishes @p event on the bus of the game at @p game, then
   * delivers it, as the games module does in the outputs stage. */
  template <typename Event>
  void publish(std::size_t game, Event event) {
    rtype::engine::EventBus& events = *games_->buses()[game];
    events.publish(event);
    events.dispatch();
  }

  /** @brief Runs the stages of the application @p ticks times, one
   * simulation tick each. */
  void runTicks(std::size_t ticks) const;

  /** @brief Stops the application, as Ctrl-C does. */
  void stop();

  /** @returns The text of STATISTICS_FILE_NAME in the temporary folder. */
  [[nodiscard]] std::string fileText() const;

  [[nodiscard]] const RecordingJournal& journal() const;
  [[nodiscard]] const std::filesystem::path& folder() const;

 private:
  /** @brief Builds and starts the application, its configuration running the
   * plugin lines, then @p lines. */
  void startWith(const std::vector<std::string>& lines);

  TemporaryFolder folder_;
  PluginHostState host_;
  GameBusesModule* games_{nullptr};
  std::unique_ptr<rtype::engine::Application> application_;
};
