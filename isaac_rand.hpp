#ifndef ISAAC_RANDOM_H
#define ISAAC_RANDOM_H
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>

/*
------------------------------------------------------------------------------
readable.c: My random number generator, ISAAC.
(c) Bob Jenkins, March 1996, Public Domain
You may use this code in any way you wish, and it is free.  No warrantee.
* May 2008 -- made it not depend on standard.h
------------------------------------------------------------------------------
*/

// http://www.burtleburtle.net/bob/rand/isaacafa.html

class isaac_engine // ISAAC32
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 0;

	explicit isaac_engine(result_type value = default_seed)
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

	friend bool operator==(const isaac_engine &, const isaac_engine &);
	friend std::ostream& operator<<(std::ostream &, const isaac_engine &);
	friend std::istream& operator>>(std::istream &, isaac_engine &);

private:
	using ub4 = uint32_t; // unsigned 4-byte quantity

	// external results
	std::array<ub4, 256> randrsl;
	size_t randcnt;

	// internal state
	std::array<ub4, 256> mm;
	ub4 aa, bb, cc;

	void isaac()
	{
		ub4 x, y;

		cc++;     // cc just gets incremented once per 256 results
		bb += cc; // then combined with bb

		for (int i = 0; i < 256; ++i) {
			x = mm[i];
			switch (i & 3) {
			case 0: aa ^= aa << 13; break;
			case 1: aa ^= aa >> 6;  break;
			case 2: aa ^= aa << 2;  break;
			case 3: aa ^= aa >> 16; break;
			}
			aa += mm[(i + 128) & 255];
			mm[i] = y = mm[(x >> 2) & 255] + aa + bb;
			randrsl[i] = bb = mm[(y >> 10) & 255] + x;
		}
	}

	template <typename T>
	inline void mix(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h)
	{
		a ^= b << 11; d += a; b += c;
		b ^= c >> 2;  e += b; c += d;
		c ^= d << 8;  f += c; d += e;
		d ^= e >> 16; g += d; e += f;
		e ^= f << 10; h += e; f += g;
		f ^= g >> 4;  a += f; g += h;
		g ^= h << 8;  b += g; h += a;
		h ^= a >> 9;  c += h; a += b;
	}

	void randinit(bool flag)
	{
		aa = bb = cc = 0;
		ub4 a, b, c, d, e, f, g, h;
		a = b = c = d = e = f = g = h = 0x9e3779b9; // the golden ratio

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

bool operator==(const isaac_engine &lhs, const isaac_engine &rhs)
{
	return (lhs.randrsl == rhs.randrsl)
		&& (lhs.randcnt == rhs.randcnt)
		&& (lhs.mm == rhs.mm)
		&& (lhs.aa == rhs.aa)
		&& (lhs.bb == rhs.bb)
		&& (lhs.cc == rhs.cc);
}
bool operator!=(const isaac_engine &lhs, const isaac_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const isaac_engine &eng)
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
std::istream& operator>>(std::istream &is, isaac_engine &eng)
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

#endif // ISAAC_RANDOM_H
