#include "UpdateScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <string>
#include <utility>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"
#include "UpdateConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateScreenConstants.hpp"
#include "UpdateScreenParts.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

namespace {

sf::IntRect centeredPanel(sf::Vector2i size) {
  return {(INTERFACE_AREA.size - size) / SIDES_PER_AXIS, size};
}

/** @returns How many of @p width pixels the share of the package received
 * fills. */
int filledWidth(int width, const UpdateProgress& progress) {
  return width * progress.percent / FULL_DOWNLOAD_PERCENT;
}

/** @brief Paints the bar at @p topLeft, filled over the share of the package
 * @p progress received. */
void paintBar(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
              const UpdateProgress& progress) {
  const sf::Vector2i border{UPDATE_BAR_BORDER, UPDATE_BAR_BORDER};
  const sf::Vector2i inside = UPDATE_BAR_SIZE - border - border;
  canvas.fill({topLeft, UPDATE_BAR_SIZE}, UPDATE_BAR_BORDER_COLOR);
  canvas.fill({topLeft + border, inside}, PANEL_INSIDE_COLOR);
  canvas.fill({topLeft + border, {filledWidth(inside.x, progress), inside.y}},
              UPDATE_BAR_FILL_COLOR);
}

/** @returns The line under the bar for @p progress. */
std::string statusLineOf(const UpdateProgress& progress) {
  switch (progress.phase) {
    case UpdatePhase::Downloading:
      return std::string{DOWNLOADING_TEXT} + std::to_string(progress.percent) +
             std::string{PERCENT_SIGN};
    case UpdatePhase::Installing:
      return std::string{INSTALLING_TEXT};
    case UpdatePhase::ReadyToRestart:
      return std::string{RESTARTING_TEXT};
    case UpdatePhase::Failed:
      return std::string{UPDATE_FAILED_TEXT};
  }
  return {};
}

}  // namespace

UpdateScreen::UpdateScreen(UpdateScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      navigator_(&parts.navigator.get()),
      run_(&parts.run.get()),
      versionLine_(std::move(parts.versionLine)) {
  repaint(run_->progress());
}

void UpdateScreen::handleEvent([[maybe_unused]] const sf::Event& event) {}

void UpdateScreen::update(engine::Duration elapsed) {
  const UpdatePhase phase = run_->progress().phase;
  inPhase_ = phase == shownPhase_ ? inPhase_ + elapsed : engine::Duration{};
  shownPhase_ = phase;
  endIfDue(phase);
  repaint(run_->progress());
}

void UpdateScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool UpdateScreen::hidesScreensBelow() const { return false; }

void UpdateScreen::endIfDue(UpdatePhase phase) {
  if (ended_) {
    return;
  }
  if (phase == UpdatePhase::ReadyToRestart &&
      inPhase_ >= RESTART_MESSAGE_DURATION && run_->restart()) {
    ended_ = true;
    navigator_->closeAll();
  } else if (phase == UpdatePhase::Failed &&
             inPhase_ >= FAILED_MESSAGE_DURATION) {
    ended_ = true;
    navigator_->pop();
  }
}

void UpdateScreen::repaint(const UpdateProgress& progress) {
  const sf::IntRect panel = centeredPanel(DIALOG_SIZE);
  canvas_->clear();
  canvas_->fill(INTERFACE_AREA, DIM_LAYER_COLOR);
  CutCornerBox(panel,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(*canvas_);
  canvas_->write(UPDATE_HEADING, panel.position + DIALOG_HEADING_OFFSET,
                 DIALOG_HEADING_STYLE);
  canvas_->write(versionLine_, panel.position + DIALOG_MESSAGE_OFFSET,
                 DIALOG_MESSAGE_STYLE);
  paintBar(*canvas_, panel.position + UPDATE_BAR_OFFSET, progress);
  canvas_->write(statusLineOf(progress), panel.position + UPDATE_STATUS_OFFSET,
                 STATUS_STYLE);
  canvas_->display();
}

}  // namespace rtype::client
