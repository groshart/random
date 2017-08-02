#ifndef SUBTRACTIVE_RANDOM_H
#define SUBTRACTIVE_RANDOM_H
#include <array>
#include <cstdint>
#include <iosfwd>
#include <limits>

class subtractive_engine // Knuth subtractive
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed = 161803398;

	subtractive_engine(result_type s = default_seed)
	{
		seed(s);
	}
	void seed(result_type s = default_seed)
	{
		result_type k = 1;
		state[54] = s;
		for (size_t i = 0; i < 54; ++i) {
			size_t ii = (21 * (i + 1) % 55) - 1;
			state[ii] = k;
			k = s - k;
			s = state[ii];
		}
		for (int loop = 0; loop < 4; ++loop) {
			for (size_t i = 0; i < 55; ++i) {
				state[i] -= state[(i + 31) % 55];
			}
		}
		index1 = 0;
		index2 = 31;
	}
	result_type operator()()
	{
		index1 = (index1 + 1) % 55;
		index2 = (index2 + 1) % 55;
		state[index1] -= state[index2];
		return state[index1];
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const subtractive_engine &, const subtractive_engine &);
	friend std::ostream& operator<<(std::ostream &, const subtractive_engine &);
	friend std::istream& operator>>(std::istream &, subtractive_engine &);

private:
	std::array<result_type, 55> state;
	size_t index1, index2;
};

bool operator==(const subtractive_engine &lhs, const subtractive_engine &rhs)
{
	return (lhs.index1 == rhs.index1)
		&& (lhs.index2 == rhs.index2)
		&& (lhs.state == rhs.state);
}
bool operator!=(const subtractive_engine &lhs, const subtractive_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const subtractive_engine &eng)
{
	for (size_t i = 0; i < eng.state.size(); ++i) {
		os << eng.state[i] << ' ';
	}
	return os << eng.index1 << ' ' << eng.index2;
}
std::istream& operator>>(std::istream &is, subtractive_engine &eng)
{
	for (size_t i = 0; i < eng.state.size(); ++i) {
		if (!(is >> eng.state[i])) break;
	}
	return is >> eng.index1 >> eng.index2;
}

#endif // SUBTRACTIVE_RANDOM_H
