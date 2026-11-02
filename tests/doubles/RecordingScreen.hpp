#pragma once

#include <cstdint>
#include <functional>
#include <ostream>
#include <vector>
#include "IScreen.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

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
  rtype::client::ScreenId screen;
  ScreenCall call{};

  bool operator==(const JournalEntry& other) const = default;
};

/** @brief Prints a journal entry in test failure messages. */
void PrintTo(const JournalEntry& entry, std::ostream* output);

/** @brief Screen double that writes every call it receives, its building and
 * its destruction in a journal shared with the test, under the identifier it
 * stands for. */
class RecordingScreen : public rtype::client::IScreen {
 public:
  RecordingScreen(rtype::client::ScreenId screen,
                  std::vector<JournalEntry>& journal);

  /** @brief Writes the destruction in the journal. A failure to write it is
   * swallowed, since a destructor must not throw: the journal then lacks the
   * entry, and a test expecting it fails. */
  ~RecordingScreen() override;

  RecordingScreen(const RecordingScreen&) = delete;
  RecordingScreen& operator=(const RecordingScreen&) = delete;
  RecordingScreen(RecordingScreen&&) = delete;
  RecordingScreen& operator=(RecordingScreen&&) = delete;

  /** @brief Runs the action each time the screen handles an event, once the
   * event is written down. */
  void runOnEvent(std::function<void()> action);

  /** @brief Sets what hidesScreensBelow() answers, false at first. */
  void setHidesScreensBelow(bool hides);

  void handleEvent(const sf::Event& event) override;
  void update(rtype::engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

  /** @returns The time given to the last update, zero before any. */
  [[nodiscard]] rtype::engine::Duration lastElapsed() const;

 private:
  void write(ScreenCall call) const;

  rtype::client::ScreenId screen_;
  std::vector<JournalEntry>* journal_;
  std::function<void()> onEvent_;
  rtype::engine::Duration lastElapsed_{};
  bool hidesScreensBelow_ = false;
};
