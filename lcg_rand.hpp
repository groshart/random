#ifndef LCG_RAND_H
#define LCG_RAND_H
#include <random>

// 32-bit
using glibc_lcg_engine = std::linear_congruential_engine<uint_fast32_t, 0x41C64E6D, 0x3039, 0>;
using msvc_lcg_engine = std::linear_congruential_engine<uint_fast32_t, 0x343FD, 0x269EC3, 0>;

// 64-bit
using mmix_lcg_engine = std::linear_congruential_engine<uint_fast64_t, 0x5851F42D4C957F2D, 0x14057B7EF767814F, 0>;
using posix_lcg_engine = std::linear_congruential_engine<uint_fast64_t, 0x5DEECE66D, 0xB, 1ull << 48>; // slow...

#endif // LCG_RAND_H
