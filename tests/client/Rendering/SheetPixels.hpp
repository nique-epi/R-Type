#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>
#include <string_view>
#include "SpriteFrame.hpp"

/** @returns A sheet shipped in assets/, read from the source folder
 * (RTYPE_SOURCE_ASSETS_FOLDER, which the test targets that compile this file
 * define) with sf::Image, which needs no graphics card. */
sf::Image shippedSheet(std::string_view assetId);

/** @returns true when two rectangles share a pixel. */
bool overlap(rtype::game::SpriteFrame first, rtype::game::SpriteFrame second);

/** @returns true when a rectangle is not empty and lies inside an image of
 * that size. */
bool liesInside(rtype::game::SpriteFrame frame, sf::Vector2u imageSize);

/** @returns true when a pixel of the rectangle is not transparent. */
bool showsAVisiblePixel(const sf::Image& image, rtype::game::SpriteFrame frame);

/** @returns true when a pixel of the one-pixel ring around the rectangle is
 * not transparent: the rectangle cuts a drawing. */
bool hasAVisiblePixelAround(const sf::Image& image,
                            rtype::game::SpriteFrame frame);

/** @returns true when a pixel of the rectangle has exactly that color. */
bool showsColor(const sf::Image& image, rtype::game::SpriteFrame frame,
                sf::Color color);

/** @returns How many pixels of @p frame differ from the pixel at the same
 * place in @p other, a rectangle of the same size: what a version without
 * flash changes. */
int changedPixelCount(const sf::Image& image, rtype::game::SpriteFrame frame,
                      rtype::game::SpriteFrame other);

/** @returns How many pixels of the rectangle are not transparent. */
int visiblePixelCount(const sf::Image& image, rtype::game::SpriteFrame frame);

/** @returns The most pixels that are not transparent in any one of the
 * rectangles: the largest silhouette among the frames of an entity. */
int largestVisibleArea(const sf::Image& image,
                       std::span<const rtype::game::SpriteFrame> frames);
