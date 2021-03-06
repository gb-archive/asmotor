/*  Copyright 2008-2017 Carsten Elton Sorensen

    This file is part of ASMotor.

    ASMotor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ASMotor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ASMotor.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "asmotor.h"
#include "types.h"

#include <math.h>

#ifndef	PI
#define	PI	acos(-1.0)
#endif

double f2d(int32_t a)
{
	return a * (1.0 / 65536.0);
}

int32_t d2f(double a)
{
	return (int32_t)(a * 65536);
}

int32_t imuldiv(int32_t a, int32_t b, int32_t c)
{
	return (int32_t)((int64_t)a * b / c);
}

int32_t fmul(int32_t a, int32_t b)
{
	return (int32_t)(((int64_t)a * b) >> 16);
}

int32_t fdiv(int32_t a, int32_t b)
{
	return (int32_t)(((int64_t)a << 16) / b);
}

int32_t fsin(int32_t a)
{
	return d2f(sin(f2d(a) * 2 * PI));
}

int32_t fasin(int32_t a)
{
	return d2f(asin(f2d(a)) / (2 * PI));
}

int32_t fcos(int32_t a)
{
	return d2f(cos(f2d(a) * 2 * PI));
}

int32_t facos(int32_t a)
{
	return d2f(acos(f2d(a)) / (2 * PI));
}

int32_t ftan(int32_t a)
{
	return d2f(tan(f2d(a) * 2 * PI));
}

int32_t fatan(int32_t a)
{
	return d2f(atan(f2d(a)) / (2 * PI));
}

int32_t fatan2(int32_t a, int32_t b)
{
	return d2f(atan2(f2d(a), f2d(b)) / (2 * PI));
}

