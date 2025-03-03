#include <catch2/catch_amalgamated.hpp>
#include <iostream>
#include <numbers>

#include "../vmlib/mat44.hpp"

TEST_CASE("Translation Matrix", "[mat44]") {
  static constexpr float kEps_ = 1e-5f;

  using namespace Catch::Matchers;

  SECTION("Standard") {
    // Narrow projection on 1920x1080 screen size
    float x = 1.f, tx = -1.3f, y = 2.4f, ty = -0.3f, z = 3.6f, tz = -8.f;

    Mat44f translation = make_translation(Vec3f{tx, ty, tz});
    Vec4f vector = translation * Vec4f{x, y, z, 1.f};
    Vec4f result{x + tx, y + ty, z + tz, 1.f};

    for (int i = 0; i < 4; i++) {
      REQUIRE_THAT(vector[i], WithinAbs(result[i], kEps_));
    }
  }
}
