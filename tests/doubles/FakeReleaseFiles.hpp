#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "IReleaseFiles.hpp"
#include "ReleaseDownload.hpp"

/** @brief Release files the test serves by path; a save writes the content
 * served at its path, reporting half then all of it, and the paths saved are
 * kept. A path not served cannot be read or saved, as a 404. */
class FakeReleaseFiles : public rtype::client::IReleaseFiles {
 public:
  [[nodiscard]] std::optional<std::string> text(std::string_view path,
                                                std::size_t limit) override;
  [[nodiscard]] bool save(
      const rtype::client::ReleaseDownload& download) override;
  void stop() override;

  void serve(const std::string& path, const std::string& content);
  [[nodiscard]] const std::vector<std::string>& savedPaths() const;
  [[nodiscard]] bool stopped() const;

 private:
  std::map<std::string, std::string, std::less<>> served_;
  std::vector<std::string> saved_;
  bool stopped_{false};
};
