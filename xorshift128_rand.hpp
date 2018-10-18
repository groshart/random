#ifndef XORSHIFT128_RANDOM_H
#define XORSHIFT128_RANDOM_H
#include <algorithm>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include "splitmix64_rand.hpp"

/*  Written in 2014-2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

class xorshift128_engine // xorshift128+
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xorshift128_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		std::generate(std::begin(s), std::end(s), splitmix64_engine{ value });
	}
	result_type operator()()
	{
		uint64_t s1 = s[0];
		const uint64_t s0 = s[1];
		const uint64_t result = s0 + s1;
		s[0] = s0;
		s1 ^= s1 << 23; // a
		s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
		return result;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xorshift128_engine &, const xorshift128_engine &);
	friend std::ostream& operator<<(std::ostream &, const xorshift128_engine &);
	friend std::istream& operator>>(std::istream &, xorshift128_engine &);

private:
	uint64_t s[2];
};

bool operator==(const xorshift128_engine &lhs, const xorshift128_engine &rhs)
{
	return lhs.s == rhs.s;
}
bool operator!=(const xorshift128_engine &lhs, const xorshift128_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xorshift128_engine &eng)
{
	return os << eng.s[0] << ' ' << eng.s[1];
}
std::istream& operator>>(std::istream &is, xorshift128_engine &eng)
{
	return is >> eng.s[0] >> eng.s[1];
}

#endif // XORSHIFT128_RANDOM_H
