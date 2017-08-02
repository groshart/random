#ifndef MMIX_RANDOM_H
#define MMIX_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <random>

class mmix_engine // Donald Knuth
{
public:
	using result_type = uint64_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit mmix_engine(result_type s = default_seed) : engine(s) {}
	void seed(result_type s = default_seed)
	{
		engine.seed(s);
	}
	result_type operator()()
	{
		return engine();
	}
	void discard(unsigned long long z)
	{
		engine.discard(z);
	}

	friend bool operator==(const mmix_engine &, const mmix_engine &);
	friend std::ostream& operator<<(std::ostream &, const mmix_engine &);
	friend std::istream& operator>>(std::istream &, mmix_engine &);

private:
	enum : result_type { A = 6364136223846793005, C = 1442695040888963407, M = 0 };

	std::linear_congruential_engine<result_type, A, C, M> engine;
};

bool operator==(const mmix_engine &lhs, const mmix_engine &rhs)
{
	return lhs.engine == rhs.engine;
}
bool operator!=(const mmix_engine &lhs, const mmix_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const mmix_engine &eng)
{
	return os << eng.engine;
}
std::istream& operator>>(std::istream &is, mmix_engine &eng)
{
	return is >> eng.engine;
}

#endif // MMIX_RANDOM_H
