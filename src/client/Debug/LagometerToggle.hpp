#pragma once

#include <SFML/Window/Event.hpp>
#include "EventUse.hpp"

namespace rtype::client {

/**
 * @brief Shows and hides the lagometer on LAGOMETER_KEY, whatever screen is
 * shown. A key held down repeats its press: the first press toggles, and the
 * key toggles again only once released, or once the window lost the focus,
 * since its release then never comes.
 */
class LagometerToggle {
 public:
  /** @returns Kept for a press or a release of LAGOMETER_KEY, Passed for any
   * other event. */
  EventUse handle(const sf::Event& event);

  /** @returns Whether the lagometer is shown; hidden at first. */
  [[nodiscard]] bool shown() const;

 private:
  bool shown_{false};
  bool held_{false};
};

}  // namespace rtype::client
