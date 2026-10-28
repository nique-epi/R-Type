#pragma once

namespace rtype::client {

/** @brief How far an old screen turning off has gone: the rows the picture is
 * squeezed into, or the half width of the line it became; neither once the
 * screen is dark. */
struct ScreenOffLook {
  /** @brief Rows of the interface picture the squeezed picture covers, around
   * its middle row; 0 once the picture became the line. */
  int pictureRows{};
  /** @brief Half the width of the line, in pixels; 0 while the picture shows
   * and once the screen is dark. */
  int lineHalfWidth{};

  bool operator==(const ScreenOffLook&) const = default;
};

}  // namespace rtype::client
