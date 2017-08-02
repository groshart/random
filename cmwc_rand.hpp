#ifndef CMWC_RANDOM_H
#define CMWC_RANDOM_H
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <random>

class cmwc_engine // http://en.wikipedia.org/wiki/Complementary-multiply-with-carry
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 1;

	explicit cmwc_engine(result_type s = default_seed)
	{
		seed(s);
	}
	void seed(result_type s = default_seed)
	{
		std::linear_congruential_engine<uint_fast32_t, 0x343FD, 0x269EC3, 0> lcg_rand(s);
		for (size_t i = 0; i < Q.size(); ++i) {
			Q[i] = lcg_rand();
		}
		do {
			carry = lcg_rand();
		} while (carry >= CMWC_C_MAX);
		index = CMWC_CYCLE - 1;
	}
	result_type operator()()
	{
		index = (index + 1) & (CMWC_CYCLE - 1);
		uint64_t t = 18782ull * Q[index] + carry;
		carry = t >> 32;
		uint32_t x = static_cast<uint32_t>(t + carry);
		if (x < carry) {
			++x;
			++carry;
		}
		return Q[index] = 0xfffffffe - x;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const cmwc_engine &, const cmwc_engine &);
	friend std::ostream& operator<<(std::ostream &, const cmwc_engine &);
	friend std::istream& operator>>(std::istream &, cmwc_engine &);

private:
	enum : result_type { CMWC_CYCLE = 4096, CMWC_C_MAX = 809430660 };

	std::array<result_type, CMWC_CYCLE> Q;
	result_type carry, index;
};

bool operator==(const cmwc_engine &lhs, const cmwc_engine &rhs)
{
	return (lhs.carry == rhs.carry)
		&& (lhs.index == rhs.index)
		&& (lhs.Q == rhs.Q);
}
bool operator!=(const cmwc_engine &lhs, const cmwc_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const cmwc_engine &eng)
{
	for (size_t i = 0; i < eng.Q.size(); ++i) {
		os << eng.Q[i] << ' ';
	}
	return os << eng.carry << ' ' << eng.index;
}
std::istream& operator>>(std::istream &is, cmwc_engine &eng)
{
	for (size_t i = 0; i < eng.Q.size(); ++i) {
		if (!(is >> eng.Q[i])) break;
	}
	return is >> eng.carry >> eng.index;
}

#endif // CMWC_RANDOM_H
