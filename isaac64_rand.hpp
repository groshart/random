#ifndef ISAAC64_RANDOM_H
#define ISAAC64_RANDOM_H
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>

/*
------------------------------------------------------------------------------
isaac64.c: My random number generator for 64-bit machines.
By Bob Jenkins, 1996.  Public Domain.
------------------------------------------------------------------------------
*/

// http://www.burtleburtle.net/bob/rand/isaacafa.html

class isaac64_engine // ISAAC64
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 0;

	explicit isaac64_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		randrsl.fill(value);
		randinit(true);
	}
	result_type operator()()
	{
		if (!randcnt--) {
			isaac();
			randcnt = 255;
		}
		return randrsl[randcnt];
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const isaac64_engine &, const isaac64_engine &);
	friend std::ostream& operator<<(std::ostream &, const isaac64_engine &);
	friend std::istream& operator>>(std::istream &, isaac64_engine &);

private:
	using ub8 = uint64_t; // unsigned 8-byte quantity

	// external results
	std::array<ub8, 256> randrsl;
	size_t randcnt;

	// internal state
	std::array<ub8, 256> mm;
	ub8 aa, bb, cc;

	void isaac()
	{
		ub8 x, y;

		cc++;     // cc just gets incremented once per 256 results
		bb += cc; // then combined with bb

		for (int i = 0; i < 256; ++i) {
			x = mm[i];
			switch (i & 3) {
			case 0: aa = ~(aa ^ (aa << 21)); break;
			case 1: aa ^= aa >> 5;  break;
			case 2: aa ^= aa << 12; break;
			case 3: aa ^= aa >> 33; break;
			}
			aa += mm[(i + 128) & 255];
			mm[i] = y = mm[(x >> 2) & 255] + aa + bb;
			randrsl[i] = bb = mm[(y >> 10) & 255] + x;
		}
	}

	template <typename T>
	inline void mix(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h)
	{
		a -= e; f ^= h >> 9;  h += a;
		b -= f; g ^= a << 9;  a += b;
		c -= g; h ^= b >> 23; b += c;
		d -= h; a ^= c << 15; c += d;
		e -= a; b ^= d >> 14; d += e;
		f -= b; c ^= e << 20; e += f;
		g -= c; d ^= f >> 17; f += g;
		h -= d; e ^= g << 14; g += h;
	}

	void randinit(bool flag)
	{
		aa = bb = cc = 0;
		ub8 a, b, c, d, e, f, g, h;
		a = b = c = d = e = f = g = h = 0x9e3779b97f4a7c13ULL; // the golden ratio

		for (int i = 0; i < 4; ++i) { // scramble it
			mix(a, b, c, d, e, f, g, h);
		}

		for (int i = 0; i < 256; i += 8) { // fill in mm[] with messy stuff
			if (flag) { // use all the information in the seed
				a += randrsl[i];     b += randrsl[i + 1]; c += randrsl[i + 2]; d += randrsl[i + 3];
				e += randrsl[i + 4]; f += randrsl[i + 5]; g += randrsl[i + 6]; h += randrsl[i + 7];
			}
			mix(a, b, c, d, e, f, g, h);
			mm[i] = a;     mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
			mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
		}

		if (flag) { // do a second pass to make all of the seed affect all of mm
			for (int i = 0; i < 256; i += 8) {
				a += mm[i];     b += mm[i + 1]; c += mm[i + 2]; d += mm[i + 3];
				e += mm[i + 4]; f += mm[i + 5]; g += mm[i + 6]; h += mm[i + 7];
				mix(a, b, c, d, e, f, g, h);
				mm[i] = a;     mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
				mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
			}
		}

		isaac();       // fill in the first set of results
		randcnt = 256; // prepare to use the first set of results
	}
};

bool operator==(const isaac64_engine &lhs, const isaac64_engine &rhs)
{
	return (lhs.randrsl == rhs.randrsl)
		&& (lhs.randcnt == rhs.randcnt)
		&& (lhs.mm == rhs.mm)
		&& (lhs.aa == rhs.aa)
		&& (lhs.bb == rhs.bb)
		&& (lhs.cc == rhs.cc);
}
bool operator!=(const isaac64_engine &lhs, const isaac64_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const isaac64_engine &eng)
{
	for (auto value : eng.randrsl) {
		os << value << ' ';
	}
	os << eng.randcnt << ' ';
	for (auto value : eng.mm) {
		os << value << ' ';
	}
	os << eng.aa << ' ' << eng.bb << ' ' << eng.cc;
}
std::istream& operator>>(std::istream &is, isaac64_engine &eng)
{
	for (auto &value : eng.randrsl) {
		is >> value;
	}
	is >> eng.randcnt;
	for (auto &value : eng.mm) {
		is >> value;
	}
	is >> eng.aa >> eng.bb >> eng.cc;
}

#endif // ISAAC64_RANDOM_H
