#ifndef XOROSHIRO128_RANDOM_H
#define XOROSHIRO128_RANDOM_H
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <algorithm>
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include "splitmix64_rand.hpp"

/*  Written in 2016 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

// http://xoroshiro.di.unimi.it/

namespace
{
	uint64_t rotl(const uint64_t x, int k)
	{
#ifdef _MSC_VER
		return _rotl64(x, k);
#else
		return (x << k) | (x >> (64 - k));
#endif
	}
}

class xoroshiro128_engine // xoroshiro128+
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xoroshiro128_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		std::generate(begin(s), end(s), splitmix64_engine{ value });
	}
	result_type operator()()
	{
		const uint64_t s0 = s[0];
		uint64_t s1 = s[1];
		const uint64_t result = s0 + s1;

		s1 ^= s0;
		s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
		s[1] = rotl(s1, 36); // c

		return result;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xoroshiro128_engine &, const xoroshiro128_engine &);
	friend std::ostream& operator<<(std::ostream &, const xoroshiro128_engine &);
	friend std::istream& operator>>(std::istream &, xoroshiro128_engine &);

private:
	std::array<uint64_t, 2> s;
};

bool operator==(const xoroshiro128_engine &lhs, const xoroshiro128_engine &rhs)
{
	return lhs.s == rhs.s;
}
bool operator!=(const xoroshiro128_engine &lhs, const xoroshiro128_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xoroshiro128_engine &eng)
{
	return os << eng.s[0] << ' ' << eng.s[1];
}
std::istream& operator>>(std::istream &is, xoroshiro128_engine &eng)
{
	return is >> eng.s[0] >> eng.s[1];
}

#endif // XOROSHIRO128_RANDOM_H
