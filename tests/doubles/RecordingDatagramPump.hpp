#pragma once

#include <cstddef>
#include "IDatagramPump.hpp"

/** @brief Pump double that counts how many times it was asked to pump. */
class RecordingDatagramPump : public rtype::client::IDatagramPump {
 public:
  /** @brief Counts the call; nothing is handed out. */
  void pump() override;

  [[nodiscard]] std::size_t pumpCount() const;

 private:
  std::size_t pumpCount_ = 0;
};
