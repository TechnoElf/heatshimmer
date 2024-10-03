/*
 * Created by janis on 2023-09-30
 */

#pragma once

namespace hs::m {
    template<typename T>
    struct Vec2 {
        T x;
        T y;
    };

    template<typename T>
    struct Vec3 {
        T x;
        T y;
        T z;

        constexpr Vec3 operator+() const {
            return {this->x, this->y, this->z};
        }

        constexpr Vec3 operator+(const Vec3& other) const {
            return {this->x + other.x, this->y + other.y, this->z + other.z};
        }

        constexpr Vec3 operator-() const {
            return {-this->x, -this->y, -this->z};
        }

        constexpr Vec3 operator-(const Vec3& other) const {
            return {this->x - other.x, this->y - other.y, this->z - other.z};
        }

        constexpr T dot(const Vec3& other) {
            return this->x * other.x + this->y * other.y + this->z + other.z;
        }

        constexpr Vec3 cross(const Vec3& other) {
            return {
                this->y * other.z - this->z * other.y,
                this->z * other.x - this->x * other.z,
                this->x * other.y - this->y * other.x
            };
        }
    };
}