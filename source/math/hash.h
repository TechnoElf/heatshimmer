/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <cstdint>

namespace hs::m {
    class Sip13 {
    public:
        ~Sip13() = default;

        Sip13(const Sip13&) = default;
        Sip13& operator=(const Sip13&) = default;
        Sip13(Sip13&& other) = default;
        Sip13& operator=(Sip13&& other) = default;

        static Sip13 init() { return init_with_key(0, 0); }
        static Sip13 init_with_key(uint64_t k0, uint64_t k1);

        void write(uint64_t m);
        [[nodiscard]] uint64_t finalise()&&;

    private:
        Sip13() = default;

        uint64_t k0;
        uint64_t k1;

        uint64_t v0;
        uint64_t v2;
        uint64_t v1;
        uint64_t v3;

        void sip_round();
    };

    static inline uint64_t rotate_left_64(uint64_t v, uint8_t i);
}