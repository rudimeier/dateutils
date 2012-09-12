/*** yd.c -- guts for yd dates that consist of a year and a year-day
 *
 * Copyright (C) 2010-2012 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of dateutils.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/
/* for convenience year related stuff will be collected in here as well */

/* set aspect temporarily */
#define ASPECT_YD
/* permanent aspect, to be read as have we ever seen aspect_yd */
#if !defined ASPECT_YD_
#define ASPECT_YD_
#endif	/* !ASPECT_YD_ */

/* algo choices for jan01 wd determination */
#if defined GET_JAN01_WDAY_FULL_LOOKUP
#elif defined GET_JAN01_WDAY_28Y_LOOKUP
#elif defined GET_JAN01_WDAY_28Y_SWITCH
#elif defined GET_JAN01_WDAY_SAKAMOTO
#else
# define GET_JAN01_WDAY_28Y_LOOKUP
#endif

/* year + mon-dom -> yd algos */
#if defined YMD_GET_YD_LOOKUP
#elif defined YMD_GET_YD_DIVREM
#else
# define YMD_GET_YD_LOOKUP
#endif

/* yd -> mon-dom algos */
#if defined GET_MD_FREUNDT
#elif defined GET_MD_TRIMESTER
#else
# define GET_MD_FREUNDT
#endif


#if !defined YD_ASPECT_HELPERS_
#define YD_ASPECT_HELPERS_

#if defined GET_JAN01_WDAY_FULL_LOOKUP
# define M	(unsigned int)(DT_MONDAY)
# define T	(unsigned int)(DT_TUESDAY)
# define W	(unsigned int)(DT_WEDNESDAY)
# define R	(unsigned int)(DT_THURSDAY)
# define F	(unsigned int)(DT_FRIDAY)
# define A	(unsigned int)(DT_SATURDAY)
# define S	(unsigned int)(DT_SUNDAY)

static const __jan01_wday_block_t __jan01_wday[] = {
#define __JAN01_WDAY_BEG	(1920)
	{
		/* 1920 - 1929 */
		R, A, S, M, T, R, F, A, S, T, 0,
	}, {
		/* 1930 - 1939 */
		W, R, F, S, M, T, W, F, A, S, 0,
	}, {
		/* 1940 - 1949 */
		M, W, R, F, A, M, T, W, R, A, 0,
	}, {
		/* 1950 - 1959 */
		S, M, T, R, F, A, S, T, W, R, 0,
	}, {
		/* 1960 - 1969 */
		F, S, M, T, W, F, A, S, M, W, 0,
	}, {
		/* 1970 - 1979 */
		R, F, A, M, T, W, R, A, S, M, 0,
	}, {
		/* 1980 - 1989 */
		T, R, F, A, S, T, W, R, F, S, 0,
	}, {
		/* 1990 - 1999 */
		M, T, W, F, A, S, M, W, R, F, 0,
	}, {
		/* 2000 - 2009 */
		A, M, T, W, R, A, S, M, T, R, 0,
	}, {
		/* 2010 - 2019 */
		F, A, S, T, W, R, F, S, M, T, 0,
	}, {
		/* 2020 - 2029 */
		W, F, A, S, M, W, R, F, A, M, 0,
	}, {
		/* 2030 - 2039 */
		T, W, R, A, S, M, T, R, F, A, 0,
	}, {
		/* 2040 - 2049 */
		S, T, W, R, F, S, M, T, W, F, 0,
	}, {
		/* 2050 - 2059 */
		A, S, M, W, R, F, A, M, T, W, 0,
	}
	/* 2060 - 2069 is 1920 - 1929 */
#define __JAN01_WDAY_END	(2059)
};
# undef M
# undef T
# undef W
# undef R
# undef F
# undef A
# undef S

static inline __attribute__((pure)) __jan01_wday_block_t
__get_jan01_block(unsigned int year)
{
	return __jan01_wday[(year - __JAN01_WDAY_BEG) / __JAN01_Y_PER_B];
}

static inline __attribute__((pure)) dt_dow_t
__get_jan01_wday(unsigned int year)
{
/* get the weekday of jan01 in YEAR */
	unsigned int res;
	__jan01_wday_block_t j01b;

	if (UNLIKELY(year < __JAN01_WDAY_BEG)) {
		/* use the 140y period property */
		while ((year += 140) < __JAN01_WDAY_BEG);
	} else if (year > __JAN01_WDAY_END) {
		/* use the 140y period property */
		while ((year -= 140) > __JAN01_WDAY_END);
	}
	j01b = __get_jan01_block(year);

	switch (year % 10) {
	default:
		res = DT_MIRACLEDAY;
		break;
	case 0:
		res = j01b.y0;
		break;
	case 1:
		res = j01b.y1;
		break;
	case 2:
		res = j01b.y2;
		break;
	case 3:
		res = j01b.y3;
		break;
	case 4:
		res = j01b.y4;
		break;
	case 5:
		res = j01b.y5;
		break;
	case 6:
		res = j01b.y6;
		break;
	case 7:
		res = j01b.y7;
		break;
	case 8:
		res = j01b.y8;
		break;
	case 9:
		res = j01b.y9;
		break;
	}
	return (dt_dow_t)res;
}

#elif defined GET_JAN01_WDAY_28Y_LOOKUP
# define M	(DT_MONDAY)
# define T	(DT_TUESDAY)
# define W	(DT_WEDNESDAY)
# define R	(DT_THURSDAY)
# define F	(DT_FRIDAY)
# define A	(DT_SATURDAY)
# define S	(DT_SUNDAY)

static const dt_dow_t __jan01_28y_wday[] = {
	/* 1904 - 1910 */
	F, S, M, T, W, F, A,
	/* 1911 - 1917 */
	S, M, W, R, F, A, M,
	/* 1918 - 1924 */
	T, W, R, A, S, M, T,
	/* 1925 - 1931 */
	R, F, A, S, T, W, R,
};

# undef M
# undef T
# undef W
# undef R
# undef F
# undef A
# undef S

static __attribute__((pure)) unsigned int
__get_28y_year_equiv(unsigned year)
{
/* the 28y cycle works for 1901 to 2100, for other years find an equivalent */
	year = year % 400U;

	if (year > 300U) {
		return year + 1600U;
	} else if (year > 200U) {
		return year + 1724U;
	} else if (year > 100U) {
		return year + 1820U;
	}
	return year + 2000;
}

static inline __attribute__((pure)) dt_dow_t
__get_jan01_wday(unsigned int year)
{
/* get the weekday of jan01 in YEAR
 * using the 28y cycle thats valid till the year 2399
 * 1920 = 16 mod 28 */
#if !defined WITH_FAST_ARITH
	if (UNLIKELY(year > 2100U)) {
		year = __get_28y_year_equiv(year);
	}
#endif	/* !WITH_FAST_ARITH */
	return __jan01_28y_wday[year % 28U];
}

#elif defined GET_JAN01_WDAY_28Y_SWITCH

static inline __attribute__((pure)) dt_dow_t
__get_jan01_wday(unsigned int year)
{
/* get the weekday of jan01 in YEAR
 * using the 28y cycle thats valid till the year 2399
 * 1920 = 16 mod 28
 * switch variant */
# define M	(DT_MONDAY)
# define T	(DT_TUESDAY)
# define W	(DT_WEDNESDAY)
# define R	(DT_THURSDAY)
# define F	(DT_FRIDAY)
# define A	(DT_SATURDAY)
# define S	(DT_SUNDAY)
	switch (year % 28) {
	case 0:
		return F;
	case 1:
		return S;
	case 2:
		return M;
	case 3:
		return T;
	case 4:
		return W;
	case 5:
		return F;
	case 6:
		return A;
	case 7:
		return S;
	case 8:
		return M;
	case 9:
		return W;
	case 10:
		return R;
	case 11:
		return F;
	case 12:
		return A;
	case 13:
		return M;
	case 14:
		return T;
	case 15:
		return W;
	case 16:
		return R;
	case 17:
		return A;
	case 18:
		return S;
	case 19:
		return M;
	case 20:
		return T;
	case 21:
		return R;
	case 22:
		return F;
	case 23:
		return A;
	case 24:
		return S;
	case 25:
		return T;
	case 26:
		return W;
	case 27:
		return R;
	default:
		return DT_MIRACLEDAY;
	}
# undef M
# undef T
# undef W
# undef R
# undef F
# undef A
# undef S
}

#elif defined GET_JAN01_WDAY_SAKAMOTO

static inline __attribute__((pure)) dt_dow_t
__get_jan01_wday(unsigned int year)
{
	unsigned int res;

	year--,
		res = year + year / 4 - year / 100 + year / 400 + 1;
	return (dt_dow_t)(res % GREG_DAYS_P_WEEK);
}

#endif	/* GET_JAN01_WDAY_* */

#if defined YMD_GET_YD_LOOKUP
static inline __attribute__((pure)) unsigned int
__md_get_yday(unsigned int year, unsigned int mon, unsigned int dom)
{
	return __mon_yday[mon] + dom + UNLIKELY(__leapp(year) && mon >= 3);
}

#elif defined YMD_GET_YD_DIVREM
static inline __attribute__((pure)) unsigned int
__md_get_yday(unsigned int year, unsigned int mon, unsigned int dom)
{
#define SL(x)	((x) * 5)
#define GET_REM(x)	((rem >> SL(x)) & 0x1f)
	static const uint64_t rem =
		(19ULL << SL(0)) |
		(18ULL << SL(1)) |
		(14ULL << SL(2)) |
		(13ULL << SL(3)) |
		(11ULL << SL(4)) |
		(10ULL << SL(5)) |
		(8ULL << SL(6)) |
		(7ULL << SL(7)) |
		(6ULL << SL(8)) |
		(4ULL << SL(9)) |
		(3ULL << SL(10)) |
		(1ULL << SL(11)) |
		(0ULL << SL(12));
	return (mon - 1) * 32 + GET_REM(mon - 1) - 19 + dom +
		UNLIKELY(__leapp(year) && mon >= 3);
#undef GET_REM
#undef SL
}
#endif	/* YMD_GET_YD_* */

#if defined GET_MD_FREUNDT
/* Freundt's 32-adic algo */
static struct __md_s
__yday_get_md(unsigned int year, unsigned int doy)
{
/* Given a year and the day of the year, return gregorian month + dom
 * we use some clever maths to invert __mon_yday[] above
 * you see __mon_yday[] + 19 divrem 32 is
 *   F   0  0 19
 *   G  31  1 18
 *   H  59  2 14
 *   J  90  3 13
 *   K 120  4 11
 *   M 151  5 10
 *   N 181  6  8
 *   Q 212  7  7
 *   U 243  8  6
 *   V 273  9  4
 *   X 304 10  3
 *   Z 334 11  1
 *     365 12  0
 *
 * Now the key is to store only the remainders in a clever way.
 *
 * So in total the table we store is 5bit remainders of
 * __mon_yday[] + 19 % 32 */
#define GET_REM(x)	(rem[x])
	static const uint8_t rem[] = {
		19, 19, 18, 14, 13, 11, 10, 8, 7, 6, 4, 3, 1, 0

	};
	unsigned int m;
	unsigned int d;
	unsigned int beef;
	unsigned int cake;

	/* get 32-adic doys */
	m = (doy + 19) / 32U;
	d = (doy + 19) % 32U;
	beef = GET_REM(m);
	cake = GET_REM(m + 1);

	/* put leap years into cake */
	if (UNLIKELY(__leapp(year) && cake < 16U)) {
		/* note how all leap-affected cakes are < 16 */
		beef += beef < 16U;
		cake++;
	}

	if (d <= cake) {
		d = doy - ((m - 1) * 32 - 19 + beef);
	} else {
		d = doy - (m++ * 32 - 19 + cake);
	}
	return (struct __md_s){.m = m, .d = d};
#undef GET_REM
}

#elif defined GET_MD_TRIMESTER
/* Trimester algo, we can't figure out which one's faster */
static struct __md_s
__yday_get_md(unsigned int year, unsigned int yday)
{
/* The idea here is that the year can be divided into trimesters:
 * Mar  31   Aug  31   Jan 31
 * Apr  30   Sep  30   Feb 28/29
 * May  31   Oct  31
 * Jun  30   Nov  30
 * Jul  31   Dec  31
 *
 * The first two trimesters have 153 days each, and given one of those
 * the m/d calculation is simply:
 *   m,d <- yday divrem 30.5
 *
 * where 30.5 is achieved by:
 *   m,d <- 2 * yday divrem 61
 * of course.
 *
 * Normally given the trimester and the m,d within the trimester you
 * could assemble the m,d of a gregorian year as:
 *   m <- 5 * trimstr + m + 3
 *   d <- d
 * given 0-counting.  And wrap around months 13 and 14 to 01 and 02;
 * so from this the subtrahend in the 2nd trimester case should be
 * 153 (as opposed to -30 now).
 *
 * We employ a simpler version here that ``inserts'' 2 days after February,
 * yday 60 is 29 Feb, yday 61 is 30 Feb, then proceed as per usual until
 * the end of July where another (unnamed) 30-day month is inserted that
 * goes seamlessly with the 31,30,31,30... cycle */
	int m;
	int d;

	if ((yday -= 1 + __leapp(year)) < 59) {
		/* 3rd trimester */
		yday += __leapp(year);
	} else if ((yday += 2) < 153 + 61) {
		/* 1st trimester */
		;
	} else {
		/* 2nd trimester */
		yday += 30;
	}

	m = 2 * yday / 61;
	d = 2 * yday % 61;
	return (struct __md_s){.m = m + 1 - (m >= 7), .d = d / 2 + 1};
}
#endif	/* GET_MD_* */

#endif	/* YD_ASPECT_HELPERS_ */

#undef ASPECT_YD

/* yd.c ends here */
