#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"

/** @brief What reached a recording row control, and whether it holds the
 * input, which a test may set. */
struct RowControlRecord {
  std::vector<sf::Keyboard::Key> keys;
  int pointerPresses = 0;
  int activations = 0;
  int cancels = 0;
  bool holding = false;
};

/**
 * @brief Row control that writes down what reaches it, 40 pixels wide and
 * filled with its color: it uses the keys it is given, every key while it
 * holds the input, and every press of the pointer; it can be activated when
 * told so. Losing the input stops its hold.
 */
class RecordingRowControl : public rtype::client::RowControl {
 public:
  RecordingRowControl(RowControlRecord& record,
                      std::vector<sf::Keyboard::Key> usedKeys,
                      bool activatable);

  [[nodiscard]] int width() const override;
  void paint(rtype::client::IInterfaceCanvas& canvas, sf::IntRect area,
             rtype::client::BarState row) const override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;
  [[nodiscard]] bool holdsInput() const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;
  void cancelInput() override;

 private:
  RowControlRecord* record_;
  std::vector<sf::Keyboard::Key> usedKeys_;
  bool activatable_;
};
