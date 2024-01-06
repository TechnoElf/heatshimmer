/*
 * Created by janis on 2024-01-06
 */

#include <iostream>
#include "hash.h"

namespace hs::m {
    Sip13 Sip13::init_with_key(uint64_t k0, uint64_t k1) {
        Sip13 hasher = Sip13();
        hasher.k0 = k0;
        hasher.k1 = k1;

        hasher.v0 = k0 ^ 0x736f6d6570736575uL;
        hasher.v1 = k1 ^ 0x646f72616e646f6duL;
        hasher.v2 = k0 ^ 0x6c7967656e657261uL;
        hasher.v3 = k1 ^ 0x7465646279746573uL;

        return hasher;
    }

    uint64_t Sip13::finalise()&& {
        this->v2 ^= 0xffuL;

        this->sip_round();
        this->sip_round();
        this->sip_round();

        return this->v0 ^ this->v1 ^ this->v2 ^ this->v3;
    }

    void Sip13::write(uint64_t m) {
        this->v3 ^= m;

        this->sip_round();

        this->v0 ^= m;
    }

    void Sip13::sip_round() {
        this->v0 += v1;
        this->v2 += v3;
        this->v1 = rotate_left_64(this->v1, 13);
        this->v3 = rotate_left_64(this->v3, 16);
        this->v1 ^= this->v0;
        this->v3 ^= this->v2;
        this->v0 = rotate_left_64(this->v0, 32);
        this->v0 += v3;
        this->v2 += v1;
        this->v1 = rotate_left_64(this->v1, 17);
        this->v3 = rotate_left_64(this->v3, 21);
        this->v1 ^= this->v2;
        this->v3 ^= this->v0;
        this->v2 = rotate_left_64(this->v2, 32);
    }

    static inline uint64_t rotate_left_64(uint64_t v, uint8_t i) {
        i %= 64;
        return v << i | v >> (64 - i);
    }
}
