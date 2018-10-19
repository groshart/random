#ifndef XOSHIRO256_RANDOM_H
#define XOSHIRO256_RANDOM_H
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <algorithm>
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include "splitmix64_rand.hpp"

/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

class xoshiro256_engine // xoshiro256**
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xoshiro256_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		std::generate(std::begin(s), std::end(s), splitmix64_engine{ value });
	}
	result_type operator()()
	{
		const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;

		const uint64_t t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 45);

		return result_starstar;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xoshiro256_engine &, const xoshiro256_engine &);
	friend std::ostream& operator<<(std::ostream &, const xoshiro256_engine &);
	friend std::istream& operator>>(std::istream &, xoshiro256_engine &);

private:
	static uint64_t rotl(const uint64_t x, int k)
	{
#ifdef _MSC_VER
		return _rotl64(x, k);
#else
		return (x << k) | (x >> (64 - k));
#endif
	}

	std::array<uint64_t, 4> s;
};

bool operator==(const xoshiro256_engine &lhs, const xoshiro256_engine &rhs)
{
	return lhs.s == rhs.s;
}
bool operator!=(const xoshiro256_engine &lhs, const xoshiro256_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xoshiro256_engine &eng)
{
	return os << eng.s[0] << ' ' << eng.s[1] << ' ' << eng.s[2] << ' ' << eng.s[3];
}
std::istream& operator>>(std::istream &is, xoshiro256_engine &eng)
{
	return is >> eng.s[0] >> eng.s[1] >> eng.s[2] >> eng.s[3];
}

#endif // XOSHIRO256_RANDOM_H
