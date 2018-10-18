#ifndef JSF64_RANDOM_H
#define JSF64_RANDOM_H
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <cstdint>
#include <iosfwd>
#include <limits>

/* A Small Noncryptographic PRNG by Bob Jenkins */

// http://burtleburtle.net/bob/rand/smallprng.html
// http://www.pcg-random.org/posts/bob-jenkins-small-prng-passes-practrand.html

class jsf64_engine // jsf64
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit jsf64_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		x.a = 0xf1ea5eed, x.b = x.c = x.d = value;
		for (int i = 0; i < 20; ++i) {
			(*this)();
		}
	}
	result_type operator()()
	{
		result_type e = x.a - rotl(x.b, 7);
		x.a = x.b ^ rotl(x.c, 13);
		x.b = x.c + rotl(x.d, 37);
		x.c = x.d + e;
		x.d = e + x.a;
		return x.d;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const jsf64_engine &, const jsf64_engine &);
	friend std::ostream& operator<<(std::ostream &, const jsf64_engine &);
	friend std::istream& operator>>(std::istream &, jsf64_engine &);

private:
	static uint64_t rotl(const uint64_t x, int k)
	{
#ifdef _MSC_VER
		return _rotl64(x, k);
#else
		return (x << k) | (x >> (64 - k));
#endif
	}

	struct ranctx { result_type a, b, c, d; } x;
};

bool operator==(const jsf64_engine &lhs, const jsf64_engine &rhs)
{
	return
		lhs.x.a == rhs.x.a &&
		lhs.x.b == rhs.x.b &&
		lhs.x.c == rhs.x.c &&
		lhs.x.d == rhs.x.d;
}
bool operator!=(const jsf64_engine &lhs, const jsf64_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const jsf64_engine &eng)
{
	return os << eng.x.a << ' ' << eng.x.b << ' ' << eng.x.c << ' ' << eng.x.d;
}
std::istream& operator>>(std::istream &is, jsf64_engine &eng)
{
	return is >> eng.x.a >> eng.x.b >> eng.x.c >> eng.x.d;
}

#endif // JSF64_RANDOM_H
