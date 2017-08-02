#ifndef XORSHIFT1024_RANDOM_H
#define XORSHIFT1024_RANDOM_H
#include <algorithm>
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include "splitmix64_rand.hpp"

/*  Written in 2014-2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

// http://xoroshiro.di.unimi.it/

class xorshift1024_engine // xorshift1024*
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit xorshift1024_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		std::generate(begin(s), end(s), splitmix64_engine{ value });
		p = 0;
	}
	result_type operator()()
	{
		const uint64_t s0 = s[p];
		uint64_t s1 = s[p = (p + 1) & 15];
		s1 ^= s1 << 31; // a
		s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c
		return s[p] * 1181783497276652981ULL;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const xorshift1024_engine &, const xorshift1024_engine &);
	friend std::ostream& operator<<(std::ostream &, const xorshift1024_engine &);
	friend std::istream& operator>>(std::istream &, xorshift1024_engine &);

private:
	std::array<uint64_t, 16> s;
	int p;
};

bool operator==(const xorshift1024_engine &lhs, const xorshift1024_engine &rhs)
{
	return (lhs.p == rhs.p) && (lhs.s == rhs.s);
}
bool operator!=(const xorshift1024_engine &lhs, const xorshift1024_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const xorshift1024_engine &eng)
{
	for (auto value : eng.s) {
		os << value << ' ';
	}
	return os << eng.p;
}
std::istream& operator>>(std::istream &is, xorshift1024_engine &eng)
{
	for (auto& value : eng.s) {
		is >> value;
	}
	return is >> eng.p;
}

#endif // XORSHIFT1024_RANDOM_H
