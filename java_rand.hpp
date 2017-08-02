#ifndef JAVA_RANDOM_H
#define JAVA_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <random>

class java_engine // java.util.Random
{
private:
	using param_type = uint64_t;

public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit java_engine(param_type s = default_seed) : engine((s ^ A) & M48) {}
	void seed(param_type s = default_seed)
	{
		engine.seed((s ^ A) & M48);
	}
	result_type operator()()
	{
		return static_cast<result_type>((engine() & M48) >> SHIFT16);
	}
	void discard(unsigned long long z)
	{
		engine.discard(z);
	}

	friend bool operator==(const java_engine &, const java_engine &);
	friend std::ostream& operator<<(std::ostream &, const java_engine &);
	friend std::istream& operator>>(std::istream &, java_engine &);

private:
	enum : param_type { A = 0x5DEECE66D, C = 0xB, M = 0 }; // FIXME: C4310 warning on Visual Studio /W4
	enum : param_type { SHIFT16 = 16, M48 = (1ull << 48) - 1 }; // bits 47..16

	std::linear_congruential_engine<param_type, A, C, M> engine;
};

bool operator==(const java_engine &lhs, const java_engine &rhs)
{
	return lhs.engine == rhs.engine;
}
bool operator!=(const java_engine &lhs, const java_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const java_engine &eng)
{
	return os << eng.engine;
}
std::istream& operator>>(std::istream &is, java_engine &eng)
{
	return is >> eng.engine;
}
#endif // JAVA_RANDOM_H
