#include <catch2/catch_amalgamated.hpp>
#include <iostream>
#include <numbers>

#include "../vmlib/mat44.hpp"

TEST_CASE("Perspective projection", "[mat44]") {
  static constexpr float kEps_ = 1e-5f;

  using namespace Catch::Matchers;

  SECTION("Standard") {
    // Narrow projection on 1920x1080 screen size
    Mat44f proj = make_perspective_projection(std::numbers::pi_v<float> / 4.f,
                                              1920 / float(1080), 0.1f, 100.f);
    Mat44f result{1.358f, 0.f, 0.f,     0.f,      0.f, 2.41421f, 0.f,  0.f,
                  0.f,    0.f, -1.002f, -0.2002f, 0.f, 0.f,      -1.f, 0.f};

    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        REQUIRE_THAT(proj(row, col), WithinAbs(result(row, col), kEps_));
      }
    }
  }
}
