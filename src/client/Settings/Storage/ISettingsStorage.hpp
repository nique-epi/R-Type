#pragma once

#include "PlayerSettings.hpp"

namespace rtype::client {

/** @brief Where the settings are kept between two launches. */
class ISettingsStorage {
 public:
  virtual ~ISettingsStorage() = default;

  /**
   * @brief Keeps the settings for the next launch.
   * @returns Whether they were kept; false leaves the previous ones, if any.
   */
  virtual bool save(const PlayerSettings& settings) = 0;
};

}  // namespace rtype::client
