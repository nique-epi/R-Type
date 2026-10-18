#pragma once

#include <vector>
#include "ISettingsStorage.hpp"
#include "PlayerSettings.hpp"

/** @brief Settings storage that writes down each save asked instead of
 * writing a file, and refuses them once told to. */
class RecordingSettingsStorage : public rtype::client::ISettingsStorage {
 public:
  bool save(const rtype::client::PlayerSettings& settings) override;

  /** @brief Makes the next saves fail, or succeed again. */
  void refuseSaves(bool refused);

  /** @returns Every save asked, in order, refused ones included. */
  [[nodiscard]] const std::vector<rtype::client::PlayerSettings>& saves() const;

 private:
  std::vector<rtype::client::PlayerSettings> saves_;
  bool refused_ = false;
};
