#ifndef FP_H
#define FP_H

#include <cstddef>
#include <limits>

struct FP
{
	static constexpr int maxULP = 4;

	template<class T>
	static std::enable_if_t<!std::numeric_limits<T>::is_integer, bool> equal(T x, T y, int ulp = maxULP)
	{
		// the machine epsilon has to be scaled to the magnitude of the values used
		// and multiplied by the desired precision in ULPs (units in the last place)
		return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
			// unless the result is subnormal
			|| std::abs(x-y) < std::numeric_limits<T>::min();
	}

	/*
	** Find rational approximation to given real number
	** David Eppstein / UC Irvine / 8 Aug 1993
	**
	** With corrections from Arno Formella, May 2008
	**
	** usage: a.out r d
	**   r is real number to approx
	**   d is the maximum denominator allowed
	**
	** based on the theory of continued fractions
	** if x = a1 + 1/(a2 + 1/(a3 + 1/(a4 + ...)))
	** then best approximation is found by truncating this series
	** (with some adjustments in the last term).
	**
	** Note the fraction can be recovered as the first column of the matrix
	**  ( a1 1 ) ( a2 1 ) ( a3 1 ) ...
	**  ( 1  0 ) ( 1  0 ) ( 1  0 )
	** Instead of keeping the sequence of continued fraction terms,
	** we just keep the last partial product of these matrices.
	*/
	template<class T>
	static std::enable_if_t<!std::numeric_limits<T>::is_integer> rational(T fp, ptrdiff_t maxDenom, ptrdiff_t& num, ptrdiff_t& denom)
	{
		/* initialize matrix */
		ptrdiff_t m[2][2] = { {1, 0}, {0, 1} };
		ptrdiff_t ai = 0;

		/* loop finding terms until denom gets too big */
		while (m[1][0] *  ( ai = (ptrdiff_t)fp ) + m[1][1] <= maxDenom)
		{
			auto t = m[0][0] * ai + m[0][1];
			m[0][1] = m[0][0];
			m[0][0] = t;
			t = m[1][0] * ai + m[1][1];
			m[1][1] = m[1][0];
			m[1][0] = t;
			if(fp == (T)ai) break; // AF: division by zero
			fp = 1/(fp - (T) ai);
			if(fp > (T)std::numeric_limits<ptrdiff_t>::max()) break; // AF: representation failure
		}

		/* now remaining fp is between 0 and 1/ai */
		/* approx as either 0 or 1/m where m is max that will fit in maxden */
		ai = (maxDenom - m[1][1]) / m[1][0];
		num = m[0][0] * ai + m[0][1];
		denom = m[1][0] * ai + m[1][1];
	}
};

#endif // FP_H
