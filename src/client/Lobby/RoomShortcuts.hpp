#pragma once

#include <SFML/Window/Event.hpp>
#include <functional>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "RoomShortcutsParts.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief The chips of the waiting room that open a screen over it, the room
 * staying below, in a row from the right, BUTTON_GAP apart: RANKING,
 * LEADERBOARD_KEY, pushes the leaderboard, and HANGAR, HANGAR_KEY, the hangar.
 *
 * Each acts once per hold of its key, on its release; before it pushes its
 * screen, the room drops what it held (RoomShortcutsParts::beforeOpening),
 * since only the screen on top receives events.
 */
class RoomShortcuts {
 public:
  explicit RoomShortcuts(RoomShortcutsParts parts);

  /** @brief The actions of the chips point at the shortcuts, so they never
   * move. */
  RoomShortcuts(const RoomShortcuts&) = delete;
  RoomShortcuts& operator=(const RoomShortcuts&) = delete;
  RoomShortcuts(RoomShortcuts&&) = delete;
  RoomShortcuts& operator=(RoomShortcuts&&) = delete;
  ~RoomShortcuts() = default;

  /** @returns Whether a chip took @p event, as KeyChip::handleEvent() says;
   * losing the focus is told to every chip. */
  bool handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Lets the room drop what it held, then pushes @p screen. */
  void open(ScreenId screen);

  IScreenNavigator* navigator_;
  std::function<void()> beforeOpening_;
  KeyChip leaderboardChip_;
  KeyChip hangarChip_;
};

}  // namespace rtype::client
