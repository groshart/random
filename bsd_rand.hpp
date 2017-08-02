#ifndef BSD_RANDOM_H
#define BSD_RANDOM_H
#include <cstdint>
#include <iosfwd>
#include <limits>

class bsd_engine
{
public:
	using result_type = uint32_t;

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<int32_t>::max(); }
	static constexpr result_type default_seed = 1;

	explicit bsd_engine(result_type value = default_seed)
	{
		seed(value);
	}
	void seed(result_type value = default_seed)
	{
		state = value;
		discard(50); // to drop some "seed -> 1st value" linearity
	}
	result_type operator()()
	{
		if (state == 0) {
			state = 123459876;
		}
		uint32_t hi = state / 127773;
		uint32_t lo = state % 127773;
		int32_t t = 16807 * lo - 2836 * hi;
		if (t < 0) {
			t += 0x7fffffff;
		}
		return state = t;
	}
	void discard(unsigned long long z)
	{
		while (z--) {
			(*this)();
		}
	}

	friend bool operator==(const bsd_engine &, const bsd_engine &);
	friend std::ostream& operator<<(std::ostream &, const bsd_engine &);
	friend std::istream& operator>>(std::istream &, bsd_engine &);

private:
	result_type state;
};

bool operator==(const bsd_engine &lhs, const bsd_engine &rhs)
{
	return lhs.state == rhs.state;
}
bool operator!=(const bsd_engine &lhs, const bsd_engine &rhs)
{
	return !(lhs == rhs);
}
std::ostream& operator<<(std::ostream &os, const bsd_engine &eng)
{
	return os << eng.state;
}
std::istream& operator>>(std::istream &is, bsd_engine &eng)
{
	return is >> eng.state;
}

#endif // BSD_RANDOM_H
