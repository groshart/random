#ifndef XOROSHIRO64_RANDOM_H
#define XOROSHIRO64_RANDOM_H
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <cstdint>
#include <iosfwd>
#include <limits>
#include "splitmix64_rand.hpp"

/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

class xoroshiro64_engine // xoroshiro64**
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xoroshiro64_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		splitmix64_engine splitmix{ value };
		uint64_t entropy[1] = { splitmix() };
		memcpy(s, entropy, sizeof s);
	}
	result_type operator()()
	{
		const uint32_t s0 = s[0];
		uint32_t s1 = s[1];
		const uint32_t result_starstar = rotl(s0 * 0x9E3779BB, 5) * 5;

		s1 ^= s0;
		s[0] = rotl(s0, 26) ^ s1 ^ (s1 << 9); // a, b
		s[1] = rotl(s1, 13); // c

		return result_starstar;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xoroshiro64_engine &, const xoroshiro64_engine &);
	friend std::ostream& operator<<(std::ostream &, const xoroshiro64_engine &);
	friend std::istream& operator>>(std::istream &, xoroshiro64_engine &);

private:
	static uint32_t rotl(const uint32_t x, int k)
	{
#ifdef _MSC_VER
		return _rotl(x, k);
#else
		return (x << k) | (x >> (32 - k));
#endif
	}

	uint32_t s[2];
};

bool operator==(const xoroshiro64_engine &lhs, const xoroshiro64_engine &rhs)
{
	return lhs.s == rhs.s;
}
bool operator!=(const xoroshiro64_engine &lhs, const xoroshiro64_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xoroshiro64_engine &eng)
{
	return os << eng.s[0] << ' ' << eng.s[1];
}
std::istream& operator>>(std::istream &is, xoroshiro64_engine &eng)
{
	return is >> eng.s[0] >> eng.s[1];
}

#endif // XOROSHIRO64_RANDOM_H
