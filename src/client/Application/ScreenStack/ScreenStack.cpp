#include "ScreenStack.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>
#include "IScreen.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ScreenStack::ScreenStack(ScreenFactory factory)
    : factory_(std::move(factory)) {}

void ScreenStack::push(ScreenId screen) {
  pendingChanges_.push_back(Change{.kind = ChangeKind::Push, .screen = screen});
}

void ScreenStack::pop() {
  pendingChanges_.push_back(Change{.kind = ChangeKind::Pop});
}

void ScreenStack::replaceAll(ScreenId screen) {
  pendingChanges_.push_back(
      Change{.kind = ChangeKind::ReplaceAll, .screen = screen});
}

void ScreenStack::closeAll() {
  pendingChanges_.push_back(Change{.kind = ChangeKind::CloseAll});
}

void ScreenStack::applyPendingChanges() {
  const std::vector<Change> changes = std::exchange(pendingChanges_, {});
  for (const Change& change : changes) {
    apply(change);
  }
}

void ScreenStack::apply(const Change& change) {
  switch (change.kind) {
    case ChangeKind::Push:
      build(change.screen);
      return;
    case ChangeKind::Pop:
      if (!screens_.empty()) {
        screens_.pop_back();
      }
      return;
    case ChangeKind::ReplaceAll:
      destroyAll();
      build(change.screen);
      return;
    case ChangeKind::CloseAll:
      destroyAll();
      return;
  }
}

void ScreenStack::build(ScreenId screen) {
  if (factory_ == nullptr) {
    return;
  }
  std::unique_ptr<IScreen> built = factory_(screen);
  if (built != nullptr) {
    screens_.push_back(std::move(built));
  }
}

void ScreenStack::destroyAll() {
  while (!screens_.empty()) {
    screens_.pop_back();
  }
}

void ScreenStack::handleEvent(const sf::Event& event) {
  if (!screens_.empty()) {
    screens_.back()->handleEvent(event);
  }
}

void ScreenStack::update(engine::Duration elapsed) {
  for (const std::unique_ptr<IScreen>& screen : screens_) {
    screen->update(elapsed);
  }
}

void ScreenStack::draw(sf::RenderTarget& target) const {
  for (auto screen = firstDrawnScreen(); screen != screens_.end(); ++screen) {
    (*screen)->draw(target);
  }
}

std::vector<std::unique_ptr<IScreen>>::const_iterator
ScreenStack::firstDrawnScreen() const {
  const auto topmostHiding =
      std::ranges::find_if(screens_ | std::views::reverse,
                           [](const std::unique_ptr<IScreen>& screen) {
                             return screen->hidesScreensBelow();
                           });
  if (topmostHiding == screens_.rend()) {
    return screens_.begin();
  }
  return std::prev(topmostHiding.base());
}

bool ScreenStack::isEmpty() const { return screens_.empty(); }

}  // namespace rtype::client
