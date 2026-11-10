#pragma once

#include <string>

namespace rtype::client {

/** @brief A newer release the signed feed offers: its version, as Velopack
 * writes it, and the notes of its annotated tag, in Markdown. */
struct ReleaseOffer {
  std::string versionText;
  std::string notesMarkdown;
};

}  // namespace rtype::client
