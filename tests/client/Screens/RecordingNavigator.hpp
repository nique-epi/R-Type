#pragma once

#include <cstdint>
#include <optional>
#include <ostream>
#include <vector>
#include "IScreenNavigator.hpp"
#include "ScreenId.hpp"

/** @brief What a screen asked of the screens around it. */
enum class NavigatorRequest : std::uint8_t {
  Push,
  Pop,
  ReplaceAll,
  CloseAll,
};

/** @brief One request, with the screen it names, if any. */
struct NavigatorCall {
  NavigatorRequest request;
  std::optional<rtype::client::ScreenId> screen;

  bool operator==(const NavigatorCall& other) const = default;
};

/** @brief Prints a request in test failure messages. */
void PrintTo(const NavigatorCall& call, std::ostream* output);

/** @brief Navigator double that writes down every request, in order, and
 * applies none. */
class RecordingNavigator : public rtype::client::IScreenNavigator {
 public:
  void push(rtype::client::ScreenId screen) override;
  void pop() override;
  void replaceAll(rtype::client::ScreenId screen) override;
  void closeAll() override;

  [[nodiscard]] const std::vector<NavigatorCall>& calls() const;

 private:
  std::vector<NavigatorCall> calls_;
};
