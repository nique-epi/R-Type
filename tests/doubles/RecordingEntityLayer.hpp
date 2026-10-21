#pragma once

#include <vector>
#include "DrawJournal.hpp"
#include "IEntityLayer.hpp"
#include "SpriteFrame.hpp"

namespace sf {
class RenderTarget;
}  // namespace sf

/** @brief What a RecordingEntityLayer was asked; the test keeps it, since the
 * screen owns the layer. */
struct EntityLayerRecord {
  int paintCount = 0;
  std::vector<rtype::game::SpriteFrame> framesAtLastPaint;
  int showCount = 0;
  const sf::RenderTarget* lastShowTarget = nullptr;
  /** @brief Where the layer writes ENTITY_LAYER_LAID each time it is shown,
   * when a test shares one with other doubles. */
  DrawJournal* drawJournal = nullptr;
};

/** @brief Entity layer double that writes down what it is asked; it needs no
 * graphics card. */
class RecordingEntityLayer : public rtype::client::IEntityLayer {
 public:
  /** @param record Where the calls are written down; it must outlive the
   * layer. */
  explicit RecordingEntityLayer(EntityLayerRecord& record);

  void paint(rtype::engine::ComponentRegistry& components) override;
  void show(sf::RenderTarget& target) const override;

 private:
  EntityLayerRecord* record_;
};
