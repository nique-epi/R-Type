#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace rtype::server {

/**
 * @brief The difficulties the games of a supervisor may be played at: the
 * tables of the waves file its games read. The supervised games module keeps
 * them when it starts, before the supervisor answers any player, and they do
 * not change afterwards; none before.
 */
class HostedDifficulties {
 public:
  /** @brief Keeps @p names, in place of the names kept before. */
  void keep(std::vector<std::string> names);

  /** @returns Whether @p name is the name of one of the difficulties. */
  [[nodiscard]] bool contains(std::string_view name) const;

  /** @returns The names of the difficulties, in the order kept. */
  [[nodiscard]] const std::vector<std::string>& names() const;

 private:
  std::vector<std::string> names_;
};

}  // namespace rtype::server
