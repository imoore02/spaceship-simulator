#include <catch2/catch_amalgamated.hpp>
#include <iostream>

#include "../vmlib/mat44.hpp"

// See mat44-rotation.cpp first!

TEST_CASE("4x4 matrix by matrix multiplication", "[mat44]") {
  // TODO: implement your own tests here
  static constexpr float kEps_ = 1e-5f;
  using namespace Catch::Matchers;

  SECTION("Arbitrary Result") {
    Mat44f a{-1.2f, 3.4f, -5.6f, 7.8f, -9.0f, 1.1f, -2.2f, 3.3f,
             -4.4f, 5.5f, -6.6f, 7.7f, -8.8f, 9.9f, -0.1f, 2.0f};
    Mat44f b{-1.3f, 2.4f, -3.5f, 4.6f, -5.7f, 6.8f, -7.9f, 8.0f,
             -9.1f, 0.2f, -1.3f, 2.4f, -3.5f, 4.6f, -5.7f, 6.8f};

    Mat44f predictedResult{5.84f,   55.f,    -59.84f, 61.28f, 13.9f,   0.62f,
                           6.86f,   -15.44f, 7.48f,   60.94f, -63.36f, 60.28f,
                           -51.08f, 55.38f,  -58.68f, 52.08f};
    Mat44f result = a * b;

    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        REQUIRE_THAT(result(row, col),
                     WithinAbs(predictedResult(row, col), kEps_));
      }
    }
  }
}

TEST_CASE("4x4 matrix by vector multiplication", "[mat44][vec4]") {
  // TODO: implement your own tests here
  static constexpr float kEps_ = 1e-5f;
  using namespace Catch::Matchers;

  SECTION("Arbitrary Result") {
    Mat44f a{-0.1f, 1.2f, -2.3f, 3.4f, -4.5f, 5.6f, -6.7f, 7.8f,
             -8.9f, 9.0f, -1.1f, 2.2f, -3.3f, 4.4f, -5.5f, 6.6f};
    Vec4f b{4.f, 3.2f, -1.f, 0.f};
    Vec4f predictedResult{5.74f, 6.62f, -5.7f, 6.38f};
    Vec4f result = a * b;

    for (int i = 0; i < 4; i++) {
      REQUIRE_THAT(result[i], WithinAbs(predictedResult[i], kEps_));
    }
  }
}
