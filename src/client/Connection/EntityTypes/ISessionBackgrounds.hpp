#pragma once

#include <memory>
#include "BackgroundCatalog.hpp"

namespace rtype::client {

/** @brief The backgrounds the content of the session lays over those of the
 * client: those of the pack of its server. */
class ISessionBackgrounds {
 public:
  virtual ~ISessionBackgrounds() = default;

  /**
   * @returns The backgrounds the pack of the server brings; nullptr outside
   * a session, when the server plays the client's own content, and for a
   * pack without a background. They are shared, so what shows them keeps
   * them alive once the session is over.
   */
  [[nodiscard]] virtual std::shared_ptr<const BackgroundCatalog> backgrounds()
      const = 0;
};

}  // namespace rtype::client
