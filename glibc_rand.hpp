#ifndef GLIBC_RANDOM_H
#define GLIBC_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <random>

class glibc_engine // glibc (TYPE_0)
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return M31; }
	static constexpr result_type default_seed = 1;

	explicit glibc_engine(result_type s = default_seed) : engine(s) {}
	void seed(result_type s = default_seed)
	{
		engine.seed(s);
	}
	result_type operator()()
	{
		return engine() & M31;
	}
	void discard(unsigned long long z)
	{
		engine.discard(z);
	}

	friend bool operator==(const glibc_engine &, const glibc_engine &);
	friend std::ostream& operator<<(std::ostream &, const glibc_engine &);
	friend std::istream& operator>>(std::istream &, glibc_engine &);

private:
	enum : result_type { A = 1103515245, C = 12345, M = 0 };
	enum : result_type { M31 = (1u << 31) - 1 }; // bits 30..0

	std::linear_congruential_engine<result_type, A, C, M> engine;
};

bool operator==(const glibc_engine &lhs, const glibc_engine &rhs)
{
	return lhs.engine == rhs.engine;
}
bool operator!=(const glibc_engine &lhs, const glibc_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const glibc_engine &eng)
{
	return os << eng.engine;
}
std::istream& operator>>(std::istream &is, glibc_engine &eng)
{
	return is >> eng.engine;
}

#endif // GLIBC_RANDOM_H
