#pragma once

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <vector>
#include "IScreen.hpp"
#include "ScreenStack.hpp"

/** @brief What happened to a screen double. */
enum class ScreenCall : std::uint8_t {
  Built,
  HandleEvent,
  Update,
  Draw,
  Destroyed,
};

/** @brief One line of a screen journal: which screen, and what happened to
 * it. */
struct JournalEntry {
  std::string screen;
  ScreenCall call;

  bool operator==(const JournalEntry& other) const = default;
};

/** @brief Prints a journal entry in test failure messages. */
void PrintTo(const JournalEntry& entry, std::ostream* output);

/** @brief Screen double that writes every call it receives, its building and
 * its destruction in a journal shared with the test. */
class RecordingScreen : public rtype::client::IScreen {
 public:
  RecordingScreen(std::string name, std::vector<JournalEntry>& journal);
  ~RecordingScreen() override;

  RecordingScreen(const RecordingScreen&) = delete;
  RecordingScreen& operator=(const RecordingScreen&) = delete;
  RecordingScreen(RecordingScreen&&) = delete;
  RecordingScreen& operator=(RecordingScreen&&) = delete;

  /** @brief Runs the action each time the screen handles an event, once the
   * event is written down. */
  void runOnEvent(std::function<void()> action);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  void write(ScreenCall call) const;

  std::string name_;
  std::vector<JournalEntry>* journal_;
  std::function<void()> onEvent_;
};

/** @brief Builds a recording screen with this name, writing in this
 * journal. */
rtype::client::ScreenFactory recordingScreen(
    std::string name, std::vector<JournalEntry>& journal);
