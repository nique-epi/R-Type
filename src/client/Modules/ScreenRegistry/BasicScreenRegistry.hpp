#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include "ClientException.hpp"
#include "IScreen.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief The screens of the client by name: each module adds the ones it owns,
 * and the screen stack asks for them by identifier.
 *
 * A module adds its screens in install(), before the screens module is
 * installed; nothing is built until a screen is asked for. The registry is
 * generic over what a screen is built from, so that it can be tested without
 * the services of a running client: the client uses it as ScreenRegistry.
 *
 * @tparam Context What a builder is given when its screen is asked for.
 */
template <typename Context>
class BasicScreenRegistry {
 public:
  using Builder = std::function<std::unique_ptr<IScreen>(const Context&)>;

  /**
   * @brief Registers how to build @p screen.
   * @throws DuplicateScreenException if a builder is already registered under
   * that identifier.
   */
  void add(ScreenId screen, Builder builder) {
    if (!builders_.emplace(screen.name(), std::move(builder)).second) {
      throw DuplicateScreenException(screen.name());
    }
  }

  /** @returns The screen of @p screen built from @p context, or nothing when
   * no module registered that identifier. */
  [[nodiscard]] std::unique_ptr<IScreen> create(ScreenId screen,
                                                const Context& context) const {
    const auto builder = builders_.find(screen.name());
    if (builder == builders_.end()) {
      return nullptr;
    }
    return builder->second(context);
  }

 private:
  std::unordered_map<std::string_view, Builder> builders_;
};

}  // namespace rtype::client
