#ifndef SPLITMIX64_RANDOM_H
#define SPLITMIX64_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>

/*  Written in 2014-2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

class splitmix64_engine // SplitMix64
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit splitmix64_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		x = value;
	}
	result_type operator()()
	{
		uint64_t z = (x += 0x9E3779B97F4A7C15ULL);
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
		return z ^ (z >> 31);
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const splitmix64_engine &, const splitmix64_engine &);
	friend std::ostream& operator<<(std::ostream &, const splitmix64_engine &);
	friend std::istream& operator>>(std::istream &, splitmix64_engine &);

private:
	uint64_t x;
};

bool operator==(const splitmix64_engine &lhs, const splitmix64_engine &rhs)
{
	return lhs.x == rhs.x;
}
bool operator!=(const splitmix64_engine &lhs, const splitmix64_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const splitmix64_engine &eng)
{
	return os << eng.x;
}
std::istream& operator>>(std::istream &is, splitmix64_engine &eng)
{
	return is >> eng.x;
}

#endif // SPLITMIX64_RANDOM_H
