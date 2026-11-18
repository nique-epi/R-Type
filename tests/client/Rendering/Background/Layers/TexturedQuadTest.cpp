#include <gtest/gtest.h>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "LayerDrawingTestConstants.hpp"
#include "TexturedQuad.hpp"

using rtype::client::appendQuad;

/**
 * Given no triangle yet
 * When a rectangle of 3 by 3 pixels at (99, 49), textured from the right
 * half of an image, is added
 * Then there are two triangles, the first corner its top left and the third
 * its bottom right, each textured from the same corner of that half
 */
TEST(TexturedQuad, AddsTheTwoTrianglesOfARectangle) {
  sf::VertexArray triangles(sf::PrimitiveType::Triangles);

  appendQuad(triangles, WHERE_THE_STAR_IS_DRAWN, RIGHT_HALF_OF_THE_IMAGE);

  ASSERT_EQ(triangles.getVertexCount(), CORNERS_OF_TWO_TRIANGLES);
  EXPECT_EQ(triangles[FIRST_CORNER].position, TOP_LEFT_OF_THE_STAR);
  EXPECT_EQ(triangles[THIRD_CORNER].position, BOTTOM_RIGHT_OF_THE_STAR);
  EXPECT_EQ(triangles[FIRST_CORNER].texCoords, TOP_LEFT_OF_THE_RIGHT_HALF);
  EXPECT_EQ(triangles[THIRD_CORNER].texCoords, BOTTOM_RIGHT_OF_THE_RIGHT_HALF);
}

/**
 * Given the two triangles of one rectangle
 * When a second rectangle is added
 * Then the first two stay, followed by two more
 */
TEST(TexturedQuad, AddsAfterTheTrianglesAlreadyThere) {
  sf::VertexArray triangles(sf::PrimitiveType::Triangles);
  appendQuad(triangles, WHERE_THE_STAR_IS_DRAWN, RIGHT_HALF_OF_THE_IMAGE);

  appendQuad(triangles, RIGHT_HALF_OF_THE_IMAGE, RIGHT_HALF_OF_THE_IMAGE);

  ASSERT_EQ(triangles.getVertexCount(), 2 * CORNERS_OF_TWO_TRIANGLES);
  EXPECT_EQ(triangles[FIRST_CORNER].position, TOP_LEFT_OF_THE_STAR);
}
