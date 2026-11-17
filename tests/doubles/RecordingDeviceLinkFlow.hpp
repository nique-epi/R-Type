#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "DeviceLinkReading.hpp"
#include "IDeviceLinkFlow.hpp"

/**
 * @brief A sign-in through the web site that reaches no master: it numbers
 * the links asked, counts the cancels, keeps the token of the last sign-out,
 * and shows whatever reading the test publishes.
 */
class RecordingDeviceLinkFlow final : public rtype::client::IDeviceLinkFlow {
 public:
  std::uint64_t askLink() override;
  void askCancel() override;
  void askSignOut(const std::string& token) override;
  [[nodiscard]] std::optional<rtype::client::DeviceLinkReading> reading()
      const override;

  /** @brief Shows @p reading as where the newest link stands. */
  void publish(const rtype::client::DeviceLinkReading& reading);

  /** @returns How many links were asked, the number of the last one. */
  [[nodiscard]] std::uint64_t linksAsked() const;

  /** @returns How many cancels were asked. */
  [[nodiscard]] int cancels() const;

  /** @returns The token askSignOut() was last given; empty before. */
  [[nodiscard]] const std::string& signedOutToken() const;

 private:
  std::uint64_t linksAsked_{};
  int cancels_{};
  std::string signedOutToken_;
  std::optional<rtype::client::DeviceLinkReading> reading_;
};
