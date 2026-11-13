#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "ItemList.hpp"
#include "KeyChip.hpp"
#include "KeyNames.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardFilterDialog.hpp"
#include "LeaderboardFrame.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardScreenParts.hpp"
#include "LeaderboardTab.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TabStrip.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The best scores of a server, pushed over the title or the waiting
 * room, which go on behind it: two banners and two guards around a panel, its
 * tabs, its rows, the row of the player and the filter.
 *
 * The screen reads its rows through ILeaderboardSource, one per tab.
 * MENU_CANCEL_KEY closes it, FILTER_KEY opens the filter, PREVIOUS_TAB_KEY and
 * NEXT_TAB_KEY change tab, once however long they are held; with no server to
 * read, MENU_CONFIRM_KEY opens the connection screen over it, which comes back
 * here once the server accepted the player. Closing the screen then leaves that
 * server, since nothing else on the title holds the session. Each has its chip
 * or its tab to click. While the filter is open, it takes every event.
 *
 * The picture changes no faster than the banners move: it is repainted on an
 * event, on a new page, and every LEADERBOARD_REPAINT_TIME, rather than each
 * frame. The rows are rebuilt only for a new page.
 *
 * Its chips and its dialog point at it, so it can be neither copied nor moved.
 */
class LeaderboardScreen : public IScreen {
 public:
  /** @param canvas Picture it is painted on, never null: the screen owns it, so
   * it comes apart from the parts it borrows. */
  LeaderboardScreen(LeaderboardScreenParts parts,
                    std::unique_ptr<IInterfaceCanvas> canvas);

  LeaderboardScreen(const LeaderboardScreen&) = delete;
  LeaderboardScreen& operator=(const LeaderboardScreen&) = delete;
  LeaderboardScreen(LeaderboardScreen&&) = delete;
  LeaderboardScreen& operator=(LeaderboardScreen&&) = delete;
  /** @brief Tells its sources to stop asking. */
  ~LeaderboardScreen() override;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Asks the source of the open tab for the chosen filter, if it has
   * one. */
  void ask();

  /** @brief Opens a tab, with the click, and reads its source. */
  void openTab(std::size_t tab);

  void openFilter();

  /** @returns The page of the open tab; without a source, what it says then. */
  [[nodiscard]] const LeaderboardPage& shownPage() const;

  /** @brief Rebuilds what depends on the page: the rows, and which chips work.
   */
  void showPage(const LeaderboardPage& page);

  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  IServerConnection* connection_;
  std::vector<LeaderboardTab> tabs_;
  LeaderboardFilterChoice choice_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  KeyNaming keyName_;
  MotionPreference motion_;
  LeaderboardFrame frame_;
  LeaderboardFilterDialog filterDialog_;
  TabStrip tabStrip_;
  ItemList list_;
  KeyChip backChip_;
  KeyChip filterChip_;
  KeyChip joinChip_;
  LeaderboardPage pageWithoutSource_{.phase = LeaderboardPhase::OutOfReach,
                                     .rows = {}};
  std::size_t openTab_ = 0;
  std::uint32_t shownRevision_{};
  engine::Duration elapsed_{};
  engine::Duration untilRepaint_{};
  bool tabKeyHeld_ = false;
  bool joinedFromHere_ = false;
};

}  // namespace rtype::client
