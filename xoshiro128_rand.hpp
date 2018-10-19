#ifndef XOSHIRO128_RANDOM_H
#define XOSHIRO128_RANDOM_H
#ifdef _MSC_VER
#include <intrin.h>
#endif
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

class xoshiro128_engine // xoshiro128**
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xoshiro128_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		splitmix64_engine splitmix{ value };
		uint64_t entropy[2] = { splitmix(), splitmix() };
		memcpy(s.data(), entropy, sizeof s);
	}
	result_type operator()()
	{
		const uint32_t result_starstar = rotl(s[0] * 5, 7) * 9;

		const uint32_t t = s[1] << 9;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 11);

		return result_starstar;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xoshiro128_engine &, const xoshiro128_engine &);
	friend std::ostream& operator<<(std::ostream &, const xoshiro128_engine &);
	friend std::istream& operator>>(std::istream &, xoshiro128_engine &);

private:
	static uint32_t rotl(const uint32_t x, int k)
	{
#ifdef _MSC_VER
		return _rotl(x, k);
#else
		return (x << k) | (x >> (32 - k));
#endif
	}

	std::array<uint32_t, 4> s;
};

bool operator==(const xoshiro128_engine &lhs, const xoshiro128_engine &rhs)
{
	return lhs.s == rhs.s;
}
bool operator!=(const xoshiro128_engine &lhs, const xoshiro128_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xoshiro128_engine &eng)
{
	return os << eng.s[0] << ' ' << eng.s[1] << ' ' << eng.s[2] << ' ' << eng.s[3];
}
std::istream& operator>>(std::istream &is, xoshiro128_engine &eng)
{
	return is >> eng.s[0] >> eng.s[1] >> eng.s[2] >> eng.s[3];
}

#endif // XOSHIRO128_RANDOM_H
