#ifndef MS_RANDOM_H
#define MS_RANDOM_H
#ifdef _MSC_VER
#define _CRT_RAND_S
#include <cstdlib> // rand_s
#include <limits>
#endif
#include <cstdint>
#include <iosfwd>
#include <random>

class msvc_engine // MSVCRT
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return MASK15; }
	static constexpr result_type default_seed = 1;

	explicit msvc_engine(result_type s = default_seed) : engine(s) {}
	void seed(result_type s = default_seed)
	{
		engine.seed(s);
	}
	result_type operator()()
	{
		return (engine() >> SHIFT16) & MASK15;
	}
	void discard(unsigned long long z)
	{
		engine.discard(z);
	}

	friend bool operator==(const msvc_engine &, const msvc_engine &);
	friend std::ostream& operator<<(std::ostream &, const msvc_engine &);
	friend std::istream& operator>>(std::istream &, msvc_engine &);

private:
	enum : result_type { A = 214013, C = 2531011, M = 0 };
	enum : result_type { SHIFT16 = 16, MASK15 = (1 << 15) - 1 }; // bits 30..16

	std::linear_congruential_engine<result_type, A, C, M> engine;
};

bool operator==(const msvc_engine &lhs, const msvc_engine &rhs)
{
	return lhs.engine == rhs.engine;
}
bool operator!=(const msvc_engine &lhs, const msvc_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const msvc_engine &eng)
{
	return os << eng.engine;
}
std::istream& operator>>(std::istream &is, msvc_engine &eng)
{
	return is >> eng.engine;
}

#ifdef _MSC_VER

class msvc_rand_s_engine // RtlGenRandom
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 0xDEADC0DE;

	explicit msvc_rand_s_engine(result_type = default_seed) {}
	void seed(result_type = default_seed) {}
	result_type operator()() const
	{
		result_type value;
		if (rand_s(&value)) {
			value = result_type(-1); // error
		}
		return value;
	}
	void discard(unsigned long long) {}
};

bool operator==(const msvc_rand_s_engine &, const msvc_rand_s_engine &) { return false; }
bool operator!=(const msvc_rand_s_engine &, const msvc_rand_s_engine &) { return true; }
std::ostream& operator<<(std::ostream &os, const msvc_rand_s_engine &) { return os; }
std::istream& operator>>(std::istream &is, msvc_rand_s_engine &) { return is; }

#endif // _MSC_VER
#endif // MS_RANDOM_H
