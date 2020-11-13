#ifndef TICKER_H_
#define TICKER_H_

#include <cassert>

namespace TerminalPlot
{
	enum ROUND
	{
		TO_UPPER,
		TO_LOWER,
		AUTO
	};

	template<typename Scalar>
	class Ticker
	{
	public:
		Ticker () {}
		Ticker (const Scalar min, const Scalar max, std::string label="")
			: min_(min), max_(max)
			{
				min_rnd = round(2, min_, ROUND::TO_LOWER);
				max_rnd = round(2, max_, ROUND::TO_UPPER);
				std::cout << label << "-ticker: min~" << min_rnd << ", max~" << max_rnd << ", relevant Digits: " << relevantDigits() << std::endl;
			};
		std::vector<Scalar> ticks(std::size_t amountOfTicks = 7) const;
		Scalar min() const {return min_rnd;}
		Scalar max() const {return max_rnd;}
		int relevantDigits() const;
		Scalar round(int significant_digits, const Scalar in, ROUND mode) const;
		std::vector<std::pair<std::size_t, std::string> > labels(std::vector<Scalar> ticks, std::vector<std::size_t> ticks_int) const
			{
				std::vector<std::pair<std::size_t, std::string> > out(ticks.size());
				for (std::size_t i=0; i<ticks.size(); i++)
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(std::abs(relevantDigits())) << ticks[i];
					out[i] = std::make_pair(ticks_int[i],ss.str());
				}
				return out;
			}
	private:
		Scalar roundToLeadingDigit(const Scalar in, ROUND mode) const;
		Scalar roundToDigit(int digit, const Scalar in, ROUND mode) const;
		const Scalar min_, max_;
		Scalar min_rnd, max_rnd;
	};

	template<typename Scalar>
	std::vector<Scalar> Ticker<Scalar>::
	ticks(std::size_t amountOfTicks) const
	{
		std::vector<Scalar> ticks_(amountOfTicks+1);
		auto delta = (max_rnd-min_rnd)/amountOfTicks;
		delta = roundToLeadingDigit(delta, ROUND::TO_LOWER) ;
		ticks_[0] = min_rnd;
		std::size_t i=1;
		auto tick = ticks_[0]+delta;
		while ( i < ticks_.size() and tick <= max_rnd)
		{
			ticks_[i] = tick;
			i++;
			tick += delta;
		}
		return ticks_;
	}

	template<typename Scalar>
	int Ticker<Scalar>::
	relevantDigits() const
	{
		auto tmp = max_rnd - min_rnd;
		for (int power=-10; power<=10; power++)
		{
			Scalar transformed = std::pow(10,power) * tmp;
			// std::cout << "pow=" << power << ", trans=" << transformed << std::endl;
			if (transformed >= 1. and transformed <= 10.)
			{
				return power+1;
			}
		}
		assert(false and "Could not round sensibly...");
	}
	
	template<typename Scalar>
	Scalar Ticker<Scalar>::
	roundToLeadingDigit(const Scalar in, ROUND mode) const
	{
		if (in == Scalar(0)) {return in;}
		auto tmp=in;
		for (int power=-10; power<=10; power++)
		{
			Scalar transformed = std::pow(10,power) * tmp;
			// std::cout << "pow=" << power << ", trans=" << transformed << std::endl;
			if (transformed >= 1. and transformed <= 10.)
			{
				if (mode == ROUND::AUTO)
					return std::round(transformed) * std::pow(10,-power);
				else if (mode == ROUND::TO_UPPER)
					return std::ceil(transformed) * std::pow(10,-power);
				else if (mode == ROUND::TO_LOWER)
					return std::floor(transformed) * std::pow(10,-power);
			}
		}
		assert(false and "Could not round sensibly...");
	}

	template<typename Scalar>
	Scalar Ticker<Scalar>::
	round(int significant_digits, const Scalar in, ROUND mode) const
	{
		if (in == 0.) {return in;}

		Scalar factor = std::pow(10.0, significant_digits - std::ceil(std::log10(std::abs(in))));
		switch (mode)
		{
		case ROUND::AUTO: return std::round(in * factor) / factor;
		case ROUND::TO_UPPER: return std::ceil(in * factor) / factor;
		case ROUND::TO_LOWER: return std::floor(in * factor) / factor;
		}
		assert(false and "strange...");
	}
	
	template<typename Scalar>
	Scalar Ticker<Scalar>::
	roundToDigit(int digit, const Scalar in, ROUND mode) const
	{
		Scalar transformed = std::pow(10,digit) * in;
		switch (mode)
		{
		case ROUND::AUTO: return std::round(transformed) * std::pow(10,-digit);
		case ROUND::TO_UPPER: return std::ceil(transformed) * std::pow(10,-digit);
		case ROUND::TO_LOWER: return std::floor(transformed) * std::pow(10,-digit);
		}
	}

}

#endif
