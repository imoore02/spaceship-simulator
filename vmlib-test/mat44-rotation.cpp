#include <catch2/catch_amalgamated.hpp>
#include <iostream>
#include <numbers>

#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 rotation matrices", "[rotation][mat44]") {
  static constexpr float kEps_ = 1e-6f;

  using namespace Catch::Matchers;

  // Simple check: rotating zero degrees should yield an idenity matrix
  SECTION("Identity") {
    Mat44f rotationX0 = make_rotation_x(0);
    Mat44f rotationY0 = make_rotation_y(0);
    Mat44f rotationZ0 = make_rotation_z(0);
    Mat44f identity = kIdentity44f;

    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        REQUIRE_THAT(rotationX0(row, col),
                     WithinAbs(identity(row, col), kEps_));
        REQUIRE_THAT(rotationY0(row, col),
                     WithinAbs(identity(row, col), kEps_));
        REQUIRE_THAT(rotationZ0(row, col),
                     WithinAbs(identity(row, col), kEps_));
      }
    }
  }

  // Rotating 90 degrees = pi/2 radians.
  SECTION("90 degrees") {
    Mat44f rotationX90 = make_rotation_x(std::numbers::pi_v<float> / 2.f);
    Mat44f rotationY90 = make_rotation_y(std::numbers::pi_v<float> / 2.f);
    Mat44f rotationZ90 = make_rotation_z(std::numbers::pi_v<float> / 2.f);
    Mat44f resultX{1.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f,
                   0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f,  1.f};
    Mat44f resultY{0.f,  0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
                   -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    Mat44f resultZ{0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f,
                   0.f, 0.f,  1.f, 0.f, 0.f, 0.f, 0.f, 1.f};

    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        REQUIRE_THAT(rotationX90(row, col),
                     WithinAbs(resultX(row, col), kEps_));
        REQUIRE_THAT(rotationY90(row, col),
                     WithinAbs(resultY(row, col), kEps_));
        REQUIRE_THAT(rotationZ90(row, col),
                     WithinAbs(resultZ(row, col), kEps_));
      }
    }
  }

  // Rotating -45 degrees = -pi / 4 radians
  SECTION("-45 degrees") {
    Mat44f rotationX_45 = make_rotation_x(-std::numbers::pi_v<float> / 4.f);
    Mat44f rotationY_45 = make_rotation_y(-std::numbers::pi_v<float> / 4.f);
    Mat44f rotationZ_45 = make_rotation_z(-std::numbers::pi_v<float> / 4.f);
    float r2_inv = 1.f / sqrt(2.f);
    Mat44f resultX{1.f, 0.f,     0.f,    0.f, 0.f, r2_inv, r2_inv, 0.f,
                   0.f, -r2_inv, r2_inv, 0.f, 0.f, 0.f,    0.f,    1.f};
    Mat44f resultY{r2_inv, 0.f, -r2_inv, 0.f, 0.f, 1.f, 0.f, 0.f,
                   r2_inv, 0.f, r2_inv,  0.f, 0.f, 0.f, 0.f, 1.f};
    Mat44f resultZ{r2_inv, r2_inv, 0.f, 0.f, -r2_inv, r2_inv, 0.f, 0.f,
                   0.f,    0.f,    1.f, 0.f, 0.f,     0.f,    0.f, 1.f};

    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        REQUIRE_THAT(rotationX_45(row, col),
                     WithinAbs(resultX(row, col), kEps_));
        REQUIRE_THAT(rotationY_45(row, col),
                     WithinAbs(resultY(row, col), kEps_));
        REQUIRE_THAT(rotationZ_45(row, col),
                     WithinAbs(resultZ(row, col), kEps_));
      }
    }
  }
}
