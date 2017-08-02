#ifndef POSIX_RANDOM_H
#define POSIX_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <random>

class posix_engine // IEEE Std 1003.1
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return M48; }
	static constexpr result_type default_seed = 1;

	explicit posix_engine(result_type s = default_seed) : engine((s << SHIFT16 | SEED) & M48) {}
	void seed(result_type s = default_seed)
	{
		engine.seed((s << SHIFT16 | SEED) & M48);
	}
	result_type operator()()
	{
		return engine() & M48;
	}
	void discard(unsigned long long z)
	{
		engine.discard(z);
	}

	friend bool operator==(const posix_engine &, const posix_engine &);
	friend std::ostream& operator<<(std::ostream &, const posix_engine &);
	friend std::istream& operator>>(std::istream &, posix_engine &);

private:
	enum : result_type { A = 0x5DEECE66D, C = 0xB, M = 0 }; // FIXME: C4310 warning on Visual Studio /W4
	enum : result_type { SHIFT16 = 16, SEED = 0x330E, M48 = (1ull << 48) - 1 }; // bits 47..0

	std::linear_congruential_engine<result_type, A, C, M> engine;
};

bool operator==(const posix_engine &lhs, const posix_engine &rhs)
{
	return lhs.engine == rhs.engine;
}
bool operator!=(const posix_engine &lhs, const posix_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const posix_engine &eng)
{
	return os << eng.engine;
}
std::istream& operator>>(std::istream &is, posix_engine &eng)
{
	return is >> eng.engine;
}

namespace posix
{
	std::uniform_real_distribution<> drand48;
	std::uniform_int_distribution<long> lrand48{ 0, std::numeric_limits<int>::max() };
	std::uniform_int_distribution<long> mrand48{ std::numeric_limits<int>::min(), std::numeric_limits<int>::max() };
}
#endif // POSIX_RANDOM_H
