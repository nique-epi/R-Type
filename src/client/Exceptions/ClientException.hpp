#pragma once

#include <stdexcept>
#include <string>

namespace rtype::client {

/**
 * @brief Root of every error raised by the client.
 */
class ClientException : public std::runtime_error {
 public:
  explicit ClientException(const std::string& message);
};

/**
 * @brief A font file could not be opened, or does not hold a font.
 */
class FontNotLoadedException : public ClientException {
 public:
  /** @param file Path of the font file, as it was given to the loader. */
  explicit FontNotLoadedException(const std::string& file);
};

/**
 * @brief A render texture could not be created on the graphics card.
 */
class RenderTextureNotCreatedException : public ClientException {
 public:
  /** @param width,height Size asked for, in pixels. */
  RenderTextureNotCreatedException(unsigned int width, unsigned int height);
};

}  // namespace rtype::client
