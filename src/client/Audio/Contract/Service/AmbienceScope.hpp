#pragma once

#include "Ambience.hpp"
#include "IAudioService.hpp"

namespace rtype::client {

/**
 * @brief Holds an ambience for as long as a screen lives: building it sets the
 * ambience, destroying it puts back the one that played before, so the hangar
 * opened over the server map gives the map its space back.
 */
class AmbienceScope {
 public:
  /** @param audio The service whose ambience it sets; it must outlive the
   * scope. */
  AmbienceScope(IAudioService& audio, Ambience ambience);

  AmbienceScope(const AmbienceScope&) = delete;
  AmbienceScope& operator=(const AmbienceScope&) = delete;
  AmbienceScope(AmbienceScope&&) = delete;
  AmbienceScope& operator=(AmbienceScope&&) = delete;
  ~AmbienceScope();

 private:
  IAudioService* audio_;
  Ambience before_;
};

}  // namespace rtype::client
