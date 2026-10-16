#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <string>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "ValueBinding.hpp"

namespace rtype::client {

/**
 * @brief A value picked along a track, in steps: a sunk groove holding the
 * track, the part up to the value filled, a thumb at the value, then the value
 * and its unit, right-aligned after the groove.
 *
 * DECREASE_KEY and INCREASE_KEY move it by a step, FIRST_VALUE_KEY and
 * LAST_VALUE_KEY to its ends, PAGE_UP_KEY and PAGE_DOWN_KEY by a page. A left
 * press on the groove moves the thumb to the step nearest the pointer, and the
 * thumb follows the pointer until the button is released. While inactive() is
 * true, as when the sound is muted, the slider is drawn dimmed and writes
 * SLIDER_INACTIVE_TEXT instead of its value; it still moves.
 */
class Slider : public RowControl {
 public:
  /** @brief The values a slider takes: minimum to maximum by step, with
   * maximum - minimum a multiple of step; a page is pageSteps steps. */
  struct Range {
    int minimum{};
    int maximum{};
    int step{};
    int pageSteps{};
  };

  /**
   * @param value Where the slider reads and writes its value.
   * @param inactive Whether it is drawn dimmed, asked at each paint.
   * @param unit Text written right after the value, such as "%".
   */
  Slider(Range range, ValueBinding<int> value, std::function<bool()> inactive,
         std::string unit);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  [[nodiscard]] bool holdsInput() const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;
  void pointerMoved(sf::Vector2i pixel, sf::IntRect area) override;
  void pointerReleased(sf::Vector2i pixel, sf::IntRect area) override;
  void cancelInput() override;

 private:
  /** @brief Writes a value, kept within the range; writes nothing when the
   * value does not change. */
  void moveTo(int value) const;

  /** @returns The pixel of the track a value lies on, from the left side of
   * the groove. */
  [[nodiscard]] int trackPixelOf(int value) const;

  /** @returns The value of the step nearest to a pixel of the control. */
  [[nodiscard]] int valueAt(sf::Vector2i pixel, sf::IntRect area) const;

  Range range_;
  ValueBinding<int> value_;
  std::function<bool()> inactive_;
  std::string unit_;
  bool dragging_ = false;
};

}  // namespace rtype::client
