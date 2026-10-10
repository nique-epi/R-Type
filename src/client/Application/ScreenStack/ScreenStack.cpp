#include "ScreenStack.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "IScreen.hpp"

namespace rtype::client {

void ScreenStack::push(ScreenFactory factory) {
  pendingChanges_.push_back(
      Change{.kind = ChangeKind::Push, .factory = std::move(factory)});
}

void ScreenStack::pop() {
  pendingChanges_.push_back(
      Change{.kind = ChangeKind::Pop, .factory = nullptr});
}

void ScreenStack::replaceAll(ScreenFactory factory) {
  pendingChanges_.push_back(
      Change{.kind = ChangeKind::ReplaceAll, .factory = std::move(factory)});
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
      build(change.factory);
      return;
    case ChangeKind::Pop:
      if (!screens_.empty()) {
        screens_.pop_back();
      }
      return;
    case ChangeKind::ReplaceAll:
      while (!screens_.empty()) {
        screens_.pop_back();
      }
      build(change.factory);
      return;
  }
}

void ScreenStack::build(const ScreenFactory& factory) {
  if (factory == nullptr) {
    return;
  }
  std::unique_ptr<IScreen> screen = factory();
  if (screen != nullptr) {
    screens_.push_back(std::move(screen));
  }
}

void ScreenStack::handleEvent(const sf::Event& event) {
  if (!screens_.empty()) {
    screens_.back()->handleEvent(event);
  }
}

void ScreenStack::update() {
  for (const std::unique_ptr<IScreen>& screen : screens_) {
    screen->update();
  }
}

void ScreenStack::draw(sf::RenderTarget& target) const {
  for (const std::unique_ptr<IScreen>& screen : screens_) {
    screen->draw(target);
  }
}

bool ScreenStack::isEmpty() const { return screens_.empty(); }

}  // namespace rtype::client
