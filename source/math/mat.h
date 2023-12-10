/*
 * Created by janis on 2023-09-30
 */

#pragma once

#include "cmath"

namespace hs::m {
    struct Mat4 {
        float m[4 * 4];

        static constexpr Mat4 unit() {
            return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        }

        static constexpr Mat4 transform(float x, float y, float z) {
            return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, x, y, z, 1.0f};
        }

        static constexpr Mat4 perspective() {
            return {1.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        }

        static constexpr Mat4 rotx(float theta) {
            return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, std::cos(theta), std::sin(theta), 0.0f, 0.0f, -std::sin(theta), std::cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        }

        static constexpr Mat4 roty(float theta) {
            return {std::cos(theta), 0.0f, -std::sin(theta), 0.0f, 0.0f, 1.0, 0.0, 0.0f, std::sin(theta), 0.0f, std::cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        }

        static constexpr Mat4 rotz(float theta) {
            return {std::cos(theta), std::sin(theta), 0.0f, 0.0f, -std::sin(theta), std::cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        }

        constexpr Mat4 operator*(const Mat4& b) {
            Mat4 res = Mat4();
            for (u_int8_t x = 0; x < 4; x++) {
                for (u_int8_t y = 0; y < 4; y++) {
                    res.m[x * 4 + y] = this->m[0 * 4 + y] * b.m[x * 4 + 0] +
                        this->m[1 * 4 + y] * b.m[x * 4 + 1] +
                        this->m[2 * 4 + y] * b.m[x * 4 + 2] +
                        this->m[3 * 4 + y] * b.m[x * 4 + 3];
                }
            }
            return res;
        }
    };
}
