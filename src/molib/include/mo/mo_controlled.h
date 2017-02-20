//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================



#include <limits.h>
#include "mo_export.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4800 4244 4018)
#endif
#include <sys/types.h>
template<class T, T init_value = 0>
class auto_init
{
public:
	typedef T	primary_type_t;
	auto_init()
	{
		f_value = init_value;
	}
	auto_init(char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(signed char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(unsigned char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(int16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(uint16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(int32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(uint32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#if UINT_MAX == ULONG_MAX
	auto_init(long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init(unsigned long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
	auto_init(int64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(uint64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(float value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	auto_init(long double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	operator T () const
	{
		return f_value;
	}
	operator T ()
	{
		return f_value;
	}
	const T * Ptr() const
	{
		return &f_value;
	}
	 T * Ptr() 
	{
		return &f_value;
	}
	bool operator ! () const
	{
		return !f_value;
	}
	T operator ~ () const
	{
		return ~f_value;
	}
	T operator + () const
	{
		return +f_value;
	}
	T operator - () const
	{
		return -f_value;
	}
	auto_init& operator ++ (void)
	{
		++f_value;
		return *this;
	}
	auto_init operator ++ (int)
	{
		auto_init<T> result(*this);
		++f_value;
		return result;
	}
	auto_init& operator -- (void)
	{
		--f_value;
		return *this;
	}
	auto_init operator -- (int)
	{
		auto_init<T> result(*this);
		--f_value;
		return result;
	}
	auto_init& operator = (const auto_init& n)
	{
		f_value = n.f_value;
		return *this;
	}
	auto_init& operator = (char value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (signed char value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (unsigned char value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (int16_t value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (uint16_t value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (int32_t value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (uint32_t value)
	{
		f_value = value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator = (long value)
	{
		f_value = value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator = (unsigned long value)
	{
		f_value = value;
		return *this;
	}
#endif
	auto_init& operator = (int64_t value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (uint64_t value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (float value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (double value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator = (long double value)
	{
		f_value = value;
		return *this;
	}
	auto_init& operator *= (const auto_init& n)
	{
		f_value *= n.f_value;
		return *this;
	}
	auto_init& operator *= (char value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (signed char value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (unsigned char value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (int16_t value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (uint16_t value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (int32_t value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (uint32_t value)
	{
		f_value *= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator *= (long value)
	{
		f_value *= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator *= (unsigned long value)
	{
		f_value *= value;
		return *this;
	}
#endif
	auto_init& operator *= (int64_t value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (uint64_t value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (float value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (double value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator *= (long double value)
	{
		f_value *= value;
		return *this;
	}
	auto_init& operator /= (const auto_init& n)
	{
		f_value /= n.f_value;
		return *this;
	}
	auto_init& operator /= (char value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (signed char value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (unsigned char value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (int16_t value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (uint16_t value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (int32_t value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (uint32_t value)
	{
		f_value /= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator /= (long value)
	{
		f_value /= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator /= (unsigned long value)
	{
		f_value /= value;
		return *this;
	}
#endif
	auto_init& operator /= (int64_t value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (uint64_t value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (float value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (double value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator /= (long double value)
	{
		f_value /= value;
		return *this;
	}
	auto_init& operator %= (const auto_init& n)
	{
		f_value %= n.f_value;
		return *this;
	}
	auto_init& operator %= (char value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (signed char value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (unsigned char value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (int16_t value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (uint16_t value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (int32_t value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (uint32_t value)
	{
		f_value %= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator %= (long value)
	{
		f_value %= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator %= (unsigned long value)
	{
		f_value %= value;
		return *this;
	}
#endif
	auto_init& operator %= (int64_t value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (uint64_t value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (float value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (double value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator %= (long double value)
	{
		f_value %= value;
		return *this;
	}
	auto_init& operator += (const auto_init& n)
	{
		f_value += n.f_value;
		return *this;
	}
	auto_init& operator += (char value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (signed char value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (unsigned char value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (int16_t value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (uint16_t value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (int32_t value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (uint32_t value)
	{
		f_value += value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator += (long value)
	{
		f_value += value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator += (unsigned long value)
	{
		f_value += value;
		return *this;
	}
#endif
	auto_init& operator += (int64_t value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (uint64_t value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (float value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (double value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator += (long double value)
	{
		f_value += value;
		return *this;
	}
	auto_init& operator -= (const auto_init& n)
	{
		f_value -= n.f_value;
		return *this;
	}
	auto_init& operator -= (char value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (signed char value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (unsigned char value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (int16_t value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (uint16_t value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (int32_t value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (uint32_t value)
	{
		f_value -= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator -= (long value)
	{
		f_value -= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator -= (unsigned long value)
	{
		f_value -= value;
		return *this;
	}
#endif
	auto_init& operator -= (int64_t value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (uint64_t value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (float value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (double value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator -= (long double value)
	{
		f_value -= value;
		return *this;
	}
	auto_init& operator <<= (const auto_init& n)
	{
		f_value <<= n.f_value;
		return *this;
	}
	auto_init& operator <<= (char value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (signed char value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (unsigned char value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (int16_t value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (uint16_t value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (int32_t value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (uint32_t value)
	{
		f_value <<= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator <<= (long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator <<= (unsigned long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
	auto_init& operator <<= (int64_t value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (uint64_t value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (float value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (double value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator <<= (long double value)
	{
		f_value <<= value;
		return *this;
	}
	auto_init& operator >>= (const auto_init& n)
	{
		f_value >>= n.f_value;
		return *this;
	}
	auto_init& operator >>= (char value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (signed char value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (unsigned char value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (int16_t value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (uint16_t value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (int32_t value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (uint32_t value)
	{
		f_value >>= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator >>= (long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator >>= (unsigned long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
	auto_init& operator >>= (int64_t value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (uint64_t value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (float value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (double value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator >>= (long double value)
	{
		f_value >>= value;
		return *this;
	}
	auto_init& operator &= (const auto_init& n)
	{
		f_value &= n.f_value;
		return *this;
	}
	auto_init& operator &= (char value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (signed char value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (unsigned char value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (int16_t value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (uint16_t value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (int32_t value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (uint32_t value)
	{
		f_value &= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator &= (long value)
	{
		f_value &= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator &= (unsigned long value)
	{
		f_value &= value;
		return *this;
	}
#endif
	auto_init& operator &= (int64_t value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (uint64_t value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (float value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (double value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator &= (long double value)
	{
		f_value &= value;
		return *this;
	}
	auto_init& operator |= (const auto_init& n)
	{
		f_value |= n.f_value;
		return *this;
	}
	auto_init& operator |= (char value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (signed char value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (unsigned char value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (int16_t value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (uint16_t value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (int32_t value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (uint32_t value)
	{
		f_value |= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator |= (long value)
	{
		f_value |= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator |= (unsigned long value)
	{
		f_value |= value;
		return *this;
	}
#endif
	auto_init& operator |= (int64_t value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (uint64_t value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (float value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (double value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator |= (long double value)
	{
		f_value |= value;
		return *this;
	}
	auto_init& operator ^= (const auto_init& n)
	{
		f_value ^= n.f_value;
		return *this;
	}
	auto_init& operator ^= (char value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (signed char value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (unsigned char value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (int16_t value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (uint16_t value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (int32_t value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (uint32_t value)
	{
		f_value ^= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	auto_init& operator ^= (long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	auto_init& operator ^= (unsigned long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
	auto_init& operator ^= (int64_t value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (uint64_t value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (float value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (double value)
	{
		f_value ^= value;
		return *this;
	}
	auto_init& operator ^= (long double value)
	{
		f_value ^= value;
		return *this;
	}
	T operator * (const auto_init& n)
	{
		return f_value * n.f_value;
	}
	T operator * (char value)
	{
		return f_value * value;
	}
	T operator * (signed char value)
	{
		return f_value * value;
	}
	T operator * (unsigned char value)
	{
		return f_value * value;
	}
	T operator * (int16_t value)
	{
		return f_value * value;
	}
	T operator * (uint16_t value)
	{
		return f_value * value;
	}
	T operator * (int32_t value)
	{
		return f_value * value;
	}
	T operator * (uint32_t value)
	{
		return f_value * value;
	}
#if UINT_MAX == ULONG_MAX
	T operator * (long value)
	{
		return f_value * value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator * (unsigned long value)
	{
		return f_value * value;
	}
#endif
	T operator * (int64_t value)
	{
		return f_value * value;
	}
	T operator * (uint64_t value)
	{
		return f_value * value;
	}
	T operator * (float value)
	{
		return f_value * value;
	}
	T operator * (double value)
	{
		return f_value * value;
	}
	T operator * (long double value)
	{
		return f_value * value;
	}
	T operator / (const auto_init& n)
	{
		return f_value / n.f_value;
	}
	T operator / (char value)
	{
		return f_value / value;
	}
	T operator / (signed char value)
	{
		return f_value / value;
	}
	T operator / (unsigned char value)
	{
		return f_value / value;
	}
	T operator / (int16_t value)
	{
		return f_value / value;
	}
	T operator / (uint16_t value)
	{
		return f_value / value;
	}
	T operator / (int32_t value)
	{
		return f_value / value;
	}
	T operator / (uint32_t value)
	{
		return f_value / value;
	}
#if UINT_MAX == ULONG_MAX
	T operator / (long value)
	{
		return f_value / value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator / (unsigned long value)
	{
		return f_value / value;
	}
#endif
	T operator / (int64_t value)
	{
		return f_value / value;
	}
	T operator / (uint64_t value)
	{
		return f_value / value;
	}
	T operator / (float value)
	{
		return f_value / value;
	}
	T operator / (double value)
	{
		return f_value / value;
	}
	T operator / (long double value)
	{
		return f_value / value;
	}
	T operator % (const auto_init& n)
	{
		return f_value % n.f_value;
	}
	T operator % (char value)
	{
		return f_value % value;
	}
	T operator % (signed char value)
	{
		return f_value % value;
	}
	T operator % (unsigned char value)
	{
		return f_value % value;
	}
	T operator % (int16_t value)
	{
		return f_value % value;
	}
	T operator % (uint16_t value)
	{
		return f_value % value;
	}
	T operator % (int32_t value)
	{
		return f_value % value;
	}
	T operator % (uint32_t value)
	{
		return f_value % value;
	}
#if UINT_MAX == ULONG_MAX
	T operator % (long value)
	{
		return f_value % value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator % (unsigned long value)
	{
		return f_value % value;
	}
#endif
	T operator % (int64_t value)
	{
		return f_value % value;
	}
	T operator % (uint64_t value)
	{
		return f_value % value;
	}
	T operator % (float value)
	{
		return f_value % value;
	}
	T operator % (double value)
	{
		return f_value % value;
	}
	T operator % (long double value)
	{
		return f_value % value;
	}
	T operator + (const auto_init& n)
	{
		return f_value + n.f_value;
	}
	T operator + (char value)
	{
		return f_value + value;
	}
	T operator + (signed char value)
	{
		return f_value + value;
	}
	T operator + (unsigned char value)
	{
		return f_value + value;
	}
	T operator + (int16_t value)
	{
		return f_value + value;
	}
	T operator + (uint16_t value)
	{
		return f_value + value;
	}
	T operator + (int32_t value)
	{
		return f_value + value;
	}
	T operator + (uint32_t value)
	{
		return f_value + value;
	}
#if UINT_MAX == ULONG_MAX
	T operator + (long value)
	{
		return f_value + value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator + (unsigned long value)
	{
		return f_value + value;
	}
#endif
	T operator + (int64_t value)
	{
		return f_value + value;
	}
	T operator + (uint64_t value)
	{
		return f_value + value;
	}
	T operator + (float value)
	{
		return f_value + value;
	}
	T operator + (double value)
	{
		return f_value + value;
	}
	T operator + (long double value)
	{
		return f_value + value;
	}
	T operator - (const auto_init& n)
	{
		return f_value - n.f_value;
	}
	T operator - (char value)
	{
		return f_value - value;
	}
	T operator - (signed char value)
	{
		return f_value - value;
	}
	T operator - (unsigned char value)
	{
		return f_value - value;
	}
	T operator - (int16_t value)
	{
		return f_value - value;
	}
	T operator - (uint16_t value)
	{
		return f_value - value;
	}
	T operator - (int32_t value)
	{
		return f_value - value;
	}
	T operator - (uint32_t value)
	{
		return f_value - value;
	}
#if UINT_MAX == ULONG_MAX
	T operator - (long value)
	{
		return f_value - value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator - (unsigned long value)
	{
		return f_value - value;
	}
#endif
	T operator - (int64_t value)
	{
		return f_value - value;
	}
	T operator - (uint64_t value)
	{
		return f_value - value;
	}
	T operator - (float value)
	{
		return f_value - value;
	}
	T operator - (double value)
	{
		return f_value - value;
	}
	T operator - (long double value)
	{
		return f_value - value;
	}
	T operator << (const auto_init& n)
	{
		return f_value << n.f_value;
	}
	T operator << (char value)
	{
		return f_value << value;
	}
	T operator << (signed char value)
	{
		return f_value << value;
	}
	T operator << (unsigned char value)
	{
		return f_value << value;
	}
	T operator << (int16_t value)
	{
		return f_value << value;
	}
	T operator << (uint16_t value)
	{
		return f_value << value;
	}
	T operator << (int32_t value)
	{
		return f_value << value;
	}
	T operator << (uint32_t value)
	{
		return f_value << value;
	}
#if UINT_MAX == ULONG_MAX
	T operator << (long value)
	{
		return f_value << value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator << (unsigned long value)
	{
		return f_value << value;
	}
#endif
	T operator << (int64_t value)
	{
		return f_value << value;
	}
	T operator << (uint64_t value)
	{
		return f_value << value;
	}
	T operator << (float value)
	{
		return f_value << value;
	}
	T operator << (double value)
	{
		return f_value << value;
	}
	T operator << (long double value)
	{
		return f_value << value;
	}
	T operator >> (const auto_init& n)
	{
		return f_value >> n.f_value;
	}
	T operator >> (char value)
	{
		return f_value >> value;
	}
	T operator >> (signed char value)
	{
		return f_value >> value;
	}
	T operator >> (unsigned char value)
	{
		return f_value >> value;
	}
	T operator >> (int16_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint16_t value)
	{
		return f_value >> value;
	}
	T operator >> (int32_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint32_t value)
	{
		return f_value >> value;
	}
#if UINT_MAX == ULONG_MAX
	T operator >> (long value)
	{
		return f_value >> value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator >> (unsigned long value)
	{
		return f_value >> value;
	}
#endif
	T operator >> (int64_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint64_t value)
	{
		return f_value >> value;
	}
	T operator >> (float value)
	{
		return f_value >> value;
	}
	T operator >> (double value)
	{
		return f_value >> value;
	}
	T operator >> (long double value)
	{
		return f_value >> value;
	}
	T operator & (const auto_init& n)
	{
		return f_value & n.f_value;
	}
	T operator & (char value)
	{
		return f_value & value;
	}
	T operator & (signed char value)
	{
		return f_value & value;
	}
	T operator & (unsigned char value)
	{
		return f_value & value;
	}
	T operator & (int16_t value)
	{
		return f_value & value;
	}
	T operator & (uint16_t value)
	{
		return f_value & value;
	}
	T operator & (int32_t value)
	{
		return f_value & value;
	}
	T operator & (uint32_t value)
	{
		return f_value & value;
	}
#if UINT_MAX == ULONG_MAX
	T operator & (long value)
	{
		return f_value & value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator & (unsigned long value)
	{
		return f_value & value;
	}
#endif
	T operator & (int64_t value)
	{
		return f_value & value;
	}
	T operator & (uint64_t value)
	{
		return f_value & value;
	}
	T operator & (float value)
	{
		return f_value & value;
	}
	T operator & (double value)
	{
		return f_value & value;
	}
	T operator & (long double value)
	{
		return f_value & value;
	}
	T operator | (const auto_init& n)
	{
		return f_value | n.f_value;
	}
	T operator | (char value)
	{
		return f_value | value;
	}
	T operator | (signed char value)
	{
		return f_value | value;
	}
	T operator | (unsigned char value)
	{
		return f_value | value;
	}
	T operator | (int16_t value)
	{
		return f_value | value;
	}
	T operator | (uint16_t value)
	{
		return f_value | value;
	}
	T operator | (int32_t value)
	{
		return f_value | value;
	}
	T operator | (uint32_t value)
	{
		return f_value | value;
	}
#if UINT_MAX == ULONG_MAX
	T operator | (long value)
	{
		return f_value | value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator | (unsigned long value)
	{
		return f_value | value;
	}
#endif
	T operator | (int64_t value)
	{
		return f_value | value;
	}
	T operator | (uint64_t value)
	{
		return f_value | value;
	}
	T operator | (float value)
	{
		return f_value | value;
	}
	T operator | (double value)
	{
		return f_value | value;
	}
	T operator | (long double value)
	{
		return f_value | value;
	}
	T operator ^ (const auto_init& n)
	{
		return f_value ^ n.f_value;
	}
	T operator ^ (char value)
	{
		return f_value ^ value;
	}
	T operator ^ (signed char value)
	{
		return f_value ^ value;
	}
	T operator ^ (unsigned char value)
	{
		return f_value ^ value;
	}
	T operator ^ (int16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (int32_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint32_t value)
	{
		return f_value ^ value;
	}
#if UINT_MAX == ULONG_MAX
	T operator ^ (long value)
	{
		return f_value ^ value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator ^ (unsigned long value)
	{
		return f_value ^ value;
	}
#endif
	T operator ^ (int64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (float value)
	{
		return f_value ^ value;
	}
	T operator ^ (double value)
	{
		return f_value ^ value;
	}
	T operator ^ (long double value)
	{
		return f_value ^ value;
	}
	bool operator == (const auto_init& n)
	{
		return f_value == n.f_value;
	}
	bool operator == (char value)
	{
		return f_value == value;
	}
	bool operator == (signed char value)
	{
		return f_value == value;
	}
	bool operator == (unsigned char value)
	{
		return f_value == value;
	}
	bool operator == (int16_t value)
	{
		return f_value == value;
	}
	bool operator == (uint16_t value)
	{
		return f_value == value;
	}
	bool operator == (int32_t value)
	{
		return f_value == value;
	}
	bool operator == (uint32_t value)
	{
		return f_value == value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator == (long value)
	{
		return f_value == value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator == (unsigned long value)
	{
		return f_value == value;
	}
#endif
	bool operator == (int64_t value)
	{
		return f_value == value;
	}
	bool operator == (uint64_t value)
	{
		return f_value == value;
	}
	bool operator == (float value)
	{
		return f_value == value;
	}
	bool operator == (double value)
	{
		return f_value == value;
	}
	bool operator == (long double value)
	{
		return f_value == value;
	}
	bool operator != (const auto_init& n)
	{
		return f_value != n.f_value;
	}
	bool operator != (char value)
	{
		return f_value != value;
	}
	bool operator != (signed char value)
	{
		return f_value != value;
	}
	bool operator != (unsigned char value)
	{
		return f_value != value;
	}
	bool operator != (int16_t value)
	{
		return f_value != value;
	}
	bool operator != (uint16_t value)
	{
		return f_value != value;
	}
	bool operator != (int32_t value)
	{
		return f_value != value;
	}
	bool operator != (uint32_t value)
	{
		return f_value != value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator != (long value)
	{
		return f_value != value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator != (unsigned long value)
	{
		return f_value != value;
	}
#endif
	bool operator != (int64_t value)
	{
		return f_value != value;
	}
	bool operator != (uint64_t value)
	{
		return f_value != value;
	}
	bool operator != (float value)
	{
		return f_value != value;
	}
	bool operator != (double value)
	{
		return f_value != value;
	}
	bool operator != (long double value)
	{
		return f_value != value;
	}
	bool operator < (const auto_init& n)
	{
		return f_value < n.f_value;
	}
	bool operator < (char value)
	{
		return f_value < value;
	}
	bool operator < (signed char value)
	{
		return f_value < value;
	}
	bool operator < (unsigned char value)
	{
		return f_value < value;
	}
	bool operator < (int16_t value)
	{
		return f_value < value;
	}
	bool operator < (uint16_t value)
	{
		return f_value < value;
	}
	bool operator < (int32_t value)
	{
		return f_value < value;
	}
	bool operator < (uint32_t value)
	{
		return f_value < value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator < (long value)
	{
		return f_value < value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator < (unsigned long value)
	{
		return f_value < value;
	}
#endif
	bool operator < (int64_t value)
	{
		return f_value < value;
	}
	bool operator < (uint64_t value)
	{
		return f_value < value;
	}
	bool operator < (float value)
	{
		return f_value < value;
	}
	bool operator < (double value)
	{
		return f_value < value;
	}
	bool operator < (long double value)
	{
		return f_value < value;
	}
	bool operator <= (const auto_init& n)
	{
		return f_value <= n.f_value;
	}
	bool operator <= (char value)
	{
		return f_value <= value;
	}
	bool operator <= (signed char value)
	{
		return f_value <= value;
	}
	bool operator <= (unsigned char value)
	{
		return f_value <= value;
	}
	bool operator <= (int16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (int32_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint32_t value)
	{
		return f_value <= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator <= (long value)
	{
		return f_value <= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator <= (unsigned long value)
	{
		return f_value <= value;
	}
#endif
	bool operator <= (int64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (float value)
	{
		return f_value <= value;
	}
	bool operator <= (double value)
	{
		return f_value <= value;
	}
	bool operator <= (long double value)
	{
		return f_value <= value;
	}
	bool operator > (const auto_init& n)
	{
		return f_value > n.f_value;
	}
	bool operator > (char value)
	{
		return f_value > value;
	}
	bool operator > (signed char value)
	{
		return f_value > value;
	}
	bool operator > (unsigned char value)
	{
		return f_value > value;
	}
	bool operator > (int16_t value)
	{
		return f_value > value;
	}
	bool operator > (uint16_t value)
	{
		return f_value > value;
	}
	bool operator > (int32_t value)
	{
		return f_value > value;
	}
	bool operator > (uint32_t value)
	{
		return f_value > value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator > (long value)
	{
		return f_value > value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator > (unsigned long value)
	{
		return f_value > value;
	}
#endif
	bool operator > (int64_t value)
	{
		return f_value > value;
	}
	bool operator > (uint64_t value)
	{
		return f_value > value;
	}
	bool operator > (float value)
	{
		return f_value > value;
	}
	bool operator > (double value)
	{
		return f_value > value;
	}
	bool operator > (long double value)
	{
		return f_value > value;
	}
	bool operator >= (const auto_init& n)
	{
		return f_value >= n.f_value;
	}
	bool operator >= (char value)
	{
		return f_value >= value;
	}
	bool operator >= (signed char value)
	{
		return f_value >= value;
	}
	bool operator >= (unsigned char value)
	{
		return f_value >= value;
	}
	bool operator >= (int16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (int32_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint32_t value)
	{
		return f_value >= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator >= (long value)
	{
		return f_value >= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator >= (unsigned long value)
	{
		return f_value >= value;
	}
#endif
	bool operator >= (int64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (float value)
	{
		return f_value >= value;
	}
	bool operator >= (double value)
	{
		return f_value >= value;
	}
	bool operator >= (long double value)
	{
		return f_value >= value;
	}

#ifdef MO_DEBUG
	bool is_initialized() const
	{
		return true;
	}
#endif

private:
	T f_value;
};

typedef auto_init<bool>	zbool_t;
typedef auto_init<char>	zchar_t;
typedef auto_init<signed char>	zschar_t;
typedef auto_init<unsigned char>	zuchar_t;
typedef auto_init<int16_t>	zint16_t;
typedef auto_init<uint16_t>	zuint16_t;
typedef auto_init<int32_t>	zint32_t;
typedef auto_init<uint32_t>	zuint32_t;
#if UINT_MAX == ULONG_MAX
typedef auto_init<long>	zplain_long_t;
#endif
#if UINT_MAX == ULONG_MAX
typedef auto_init<unsigned long>	zplain_ulong_t;
#endif
typedef auto_init<int64_t>	zint64_t;
typedef auto_init<uint64_t>	zuint64_t;
typedef auto_init<size_t>	zsize_t;
typedef auto_init<time_t>	ztime_t;

template<class T>
class fauto_init
{
public:
	typedef T	primary_type_t;
	fauto_init()
	{
		f_value = 0.0;
	}
	fauto_init(char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(signed char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(unsigned char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(int16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(uint16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(int32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(uint32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#if UINT_MAX == ULONG_MAX
	fauto_init(long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init(unsigned long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
	fauto_init(int64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(uint64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(float value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	fauto_init(long double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	operator T () const
	{
		return f_value;
	}
	operator T ()
	{
		return f_value;
	}
	const T * Ptr() const
	{
		return &f_value;
	}
	 T * Ptr() 
	{
		return &f_value;
	}
	bool operator ! () const
	{
		return !f_value;
	}
	T operator ~ () const
	{
		return ~f_value;
	}
	T operator + () const
	{
		return +f_value;
	}
	T operator - () const
	{
		return -f_value;
	}
	fauto_init& operator ++ (void)
	{
		++f_value;
		return *this;
	}
	fauto_init operator ++ (int)
	{
		fauto_init<T> result(*this);
		++f_value;
		return result;
	}
	fauto_init& operator -- (void)
	{
		--f_value;
		return *this;
	}
	fauto_init operator -- (int)
	{
		fauto_init<T> result(*this);
		--f_value;
		return result;
	}
	fauto_init& operator = (const fauto_init& n)
	{
		f_value = n.f_value;
		return *this;
	}
	fauto_init& operator = (char value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (signed char value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (unsigned char value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (int16_t value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (uint16_t value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (int32_t value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (uint32_t value)
	{
		f_value = value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator = (long value)
	{
		f_value = value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator = (unsigned long value)
	{
		f_value = value;
		return *this;
	}
#endif
	fauto_init& operator = (int64_t value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (uint64_t value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (float value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (double value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator = (long double value)
	{
		f_value = value;
		return *this;
	}
	fauto_init& operator *= (const fauto_init& n)
	{
		f_value *= n.f_value;
		return *this;
	}
	fauto_init& operator *= (char value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (signed char value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (unsigned char value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (int16_t value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (uint16_t value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (int32_t value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (uint32_t value)
	{
		f_value *= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator *= (long value)
	{
		f_value *= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator *= (unsigned long value)
	{
		f_value *= value;
		return *this;
	}
#endif
	fauto_init& operator *= (int64_t value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (uint64_t value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (float value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (double value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator *= (long double value)
	{
		f_value *= value;
		return *this;
	}
	fauto_init& operator /= (const fauto_init& n)
	{
		f_value /= n.f_value;
		return *this;
	}
	fauto_init& operator /= (char value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (signed char value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (unsigned char value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (int16_t value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (uint16_t value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (int32_t value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (uint32_t value)
	{
		f_value /= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator /= (long value)
	{
		f_value /= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator /= (unsigned long value)
	{
		f_value /= value;
		return *this;
	}
#endif
	fauto_init& operator /= (int64_t value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (uint64_t value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (float value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (double value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator /= (long double value)
	{
		f_value /= value;
		return *this;
	}
	fauto_init& operator %= (const fauto_init& n)
	{
		f_value %= n.f_value;
		return *this;
	}
	fauto_init& operator %= (char value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (signed char value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (unsigned char value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (int16_t value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (uint16_t value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (int32_t value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (uint32_t value)
	{
		f_value %= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator %= (long value)
	{
		f_value %= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator %= (unsigned long value)
	{
		f_value %= value;
		return *this;
	}
#endif
	fauto_init& operator %= (int64_t value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (uint64_t value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (float value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (double value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator %= (long double value)
	{
		f_value %= value;
		return *this;
	}
	fauto_init& operator += (const fauto_init& n)
	{
		f_value += n.f_value;
		return *this;
	}
	fauto_init& operator += (char value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (signed char value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (unsigned char value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (int16_t value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (uint16_t value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (int32_t value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (uint32_t value)
	{
		f_value += value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator += (long value)
	{
		f_value += value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator += (unsigned long value)
	{
		f_value += value;
		return *this;
	}
#endif
	fauto_init& operator += (int64_t value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (uint64_t value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (float value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (double value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator += (long double value)
	{
		f_value += value;
		return *this;
	}
	fauto_init& operator -= (const fauto_init& n)
	{
		f_value -= n.f_value;
		return *this;
	}
	fauto_init& operator -= (char value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (signed char value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (unsigned char value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (int16_t value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (uint16_t value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (int32_t value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (uint32_t value)
	{
		f_value -= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator -= (long value)
	{
		f_value -= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator -= (unsigned long value)
	{
		f_value -= value;
		return *this;
	}
#endif
	fauto_init& operator -= (int64_t value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (uint64_t value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (float value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (double value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator -= (long double value)
	{
		f_value -= value;
		return *this;
	}
	fauto_init& operator <<= (const fauto_init& n)
	{
		f_value <<= n.f_value;
		return *this;
	}
	fauto_init& operator <<= (char value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (signed char value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (unsigned char value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (int16_t value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (uint16_t value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (int32_t value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (uint32_t value)
	{
		f_value <<= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator <<= (long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator <<= (unsigned long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
	fauto_init& operator <<= (int64_t value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (uint64_t value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (float value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (double value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator <<= (long double value)
	{
		f_value <<= value;
		return *this;
	}
	fauto_init& operator >>= (const fauto_init& n)
	{
		f_value >>= n.f_value;
		return *this;
	}
	fauto_init& operator >>= (char value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (signed char value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (unsigned char value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (int16_t value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (uint16_t value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (int32_t value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (uint32_t value)
	{
		f_value >>= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator >>= (long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator >>= (unsigned long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
	fauto_init& operator >>= (int64_t value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (uint64_t value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (float value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (double value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator >>= (long double value)
	{
		f_value >>= value;
		return *this;
	}
	fauto_init& operator &= (const fauto_init& n)
	{
		f_value &= n.f_value;
		return *this;
	}
	fauto_init& operator &= (char value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (signed char value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (unsigned char value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (int16_t value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (uint16_t value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (int32_t value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (uint32_t value)
	{
		f_value &= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator &= (long value)
	{
		f_value &= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator &= (unsigned long value)
	{
		f_value &= value;
		return *this;
	}
#endif
	fauto_init& operator &= (int64_t value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (uint64_t value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (float value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (double value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator &= (long double value)
	{
		f_value &= value;
		return *this;
	}
	fauto_init& operator |= (const fauto_init& n)
	{
		f_value |= n.f_value;
		return *this;
	}
	fauto_init& operator |= (char value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (signed char value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (unsigned char value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (int16_t value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (uint16_t value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (int32_t value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (uint32_t value)
	{
		f_value |= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator |= (long value)
	{
		f_value |= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator |= (unsigned long value)
	{
		f_value |= value;
		return *this;
	}
#endif
	fauto_init& operator |= (int64_t value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (uint64_t value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (float value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (double value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator |= (long double value)
	{
		f_value |= value;
		return *this;
	}
	fauto_init& operator ^= (const fauto_init& n)
	{
		f_value ^= n.f_value;
		return *this;
	}
	fauto_init& operator ^= (char value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (signed char value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (unsigned char value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (int16_t value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (uint16_t value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (int32_t value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (uint32_t value)
	{
		f_value ^= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	fauto_init& operator ^= (long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	fauto_init& operator ^= (unsigned long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
	fauto_init& operator ^= (int64_t value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (uint64_t value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (float value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (double value)
	{
		f_value ^= value;
		return *this;
	}
	fauto_init& operator ^= (long double value)
	{
		f_value ^= value;
		return *this;
	}
	T operator * (const fauto_init& n)
	{
		return f_value * n.f_value;
	}
	T operator * (char value)
	{
		return f_value * value;
	}
	T operator * (signed char value)
	{
		return f_value * value;
	}
	T operator * (unsigned char value)
	{
		return f_value * value;
	}
	T operator * (int16_t value)
	{
		return f_value * value;
	}
	T operator * (uint16_t value)
	{
		return f_value * value;
	}
	T operator * (int32_t value)
	{
		return f_value * value;
	}
	T operator * (uint32_t value)
	{
		return f_value * value;
	}
#if UINT_MAX == ULONG_MAX
	T operator * (long value)
	{
		return f_value * value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator * (unsigned long value)
	{
		return f_value * value;
	}
#endif
	T operator * (int64_t value)
	{
		return f_value * value;
	}
	T operator * (uint64_t value)
	{
		return f_value * value;
	}
	T operator * (float value)
	{
		return f_value * value;
	}
	T operator * (double value)
	{
		return f_value * value;
	}
	T operator * (long double value)
	{
		return f_value * value;
	}
	T operator / (const fauto_init& n)
	{
		return f_value / n.f_value;
	}
	T operator / (char value)
	{
		return f_value / value;
	}
	T operator / (signed char value)
	{
		return f_value / value;
	}
	T operator / (unsigned char value)
	{
		return f_value / value;
	}
	T operator / (int16_t value)
	{
		return f_value / value;
	}
	T operator / (uint16_t value)
	{
		return f_value / value;
	}
	T operator / (int32_t value)
	{
		return f_value / value;
	}
	T operator / (uint32_t value)
	{
		return f_value / value;
	}
#if UINT_MAX == ULONG_MAX
	T operator / (long value)
	{
		return f_value / value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator / (unsigned long value)
	{
		return f_value / value;
	}
#endif
	T operator / (int64_t value)
	{
		return f_value / value;
	}
	T operator / (uint64_t value)
	{
		return f_value / value;
	}
	T operator / (float value)
	{
		return f_value / value;
	}
	T operator / (double value)
	{
		return f_value / value;
	}
	T operator / (long double value)
	{
		return f_value / value;
	}
	T operator % (const fauto_init& n)
	{
		return f_value % n.f_value;
	}
	T operator % (char value)
	{
		return f_value % value;
	}
	T operator % (signed char value)
	{
		return f_value % value;
	}
	T operator % (unsigned char value)
	{
		return f_value % value;
	}
	T operator % (int16_t value)
	{
		return f_value % value;
	}
	T operator % (uint16_t value)
	{
		return f_value % value;
	}
	T operator % (int32_t value)
	{
		return f_value % value;
	}
	T operator % (uint32_t value)
	{
		return f_value % value;
	}
#if UINT_MAX == ULONG_MAX
	T operator % (long value)
	{
		return f_value % value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator % (unsigned long value)
	{
		return f_value % value;
	}
#endif
	T operator % (int64_t value)
	{
		return f_value % value;
	}
	T operator % (uint64_t value)
	{
		return f_value % value;
	}
	T operator % (float value)
	{
		return f_value % value;
	}
	T operator % (double value)
	{
		return f_value % value;
	}
	T operator % (long double value)
	{
		return f_value % value;
	}
	T operator + (const fauto_init& n)
	{
		return f_value + n.f_value;
	}
	T operator + (char value)
	{
		return f_value + value;
	}
	T operator + (signed char value)
	{
		return f_value + value;
	}
	T operator + (unsigned char value)
	{
		return f_value + value;
	}
	T operator + (int16_t value)
	{
		return f_value + value;
	}
	T operator + (uint16_t value)
	{
		return f_value + value;
	}
	T operator + (int32_t value)
	{
		return f_value + value;
	}
	T operator + (uint32_t value)
	{
		return f_value + value;
	}
#if UINT_MAX == ULONG_MAX
	T operator + (long value)
	{
		return f_value + value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator + (unsigned long value)
	{
		return f_value + value;
	}
#endif
	T operator + (int64_t value)
	{
		return f_value + value;
	}
	T operator + (uint64_t value)
	{
		return f_value + value;
	}
	T operator + (float value)
	{
		return f_value + value;
	}
	T operator + (double value)
	{
		return f_value + value;
	}
	T operator + (long double value)
	{
		return f_value + value;
	}
	T operator - (const fauto_init& n)
	{
		return f_value - n.f_value;
	}
	T operator - (char value)
	{
		return f_value - value;
	}
	T operator - (signed char value)
	{
		return f_value - value;
	}
	T operator - (unsigned char value)
	{
		return f_value - value;
	}
	T operator - (int16_t value)
	{
		return f_value - value;
	}
	T operator - (uint16_t value)
	{
		return f_value - value;
	}
	T operator - (int32_t value)
	{
		return f_value - value;
	}
	T operator - (uint32_t value)
	{
		return f_value - value;
	}
#if UINT_MAX == ULONG_MAX
	T operator - (long value)
	{
		return f_value - value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator - (unsigned long value)
	{
		return f_value - value;
	}
#endif
	T operator - (int64_t value)
	{
		return f_value - value;
	}
	T operator - (uint64_t value)
	{
		return f_value - value;
	}
	T operator - (float value)
	{
		return f_value - value;
	}
	T operator - (double value)
	{
		return f_value - value;
	}
	T operator - (long double value)
	{
		return f_value - value;
	}
	T operator << (const fauto_init& n)
	{
		return f_value << n.f_value;
	}
	T operator << (char value)
	{
		return f_value << value;
	}
	T operator << (signed char value)
	{
		return f_value << value;
	}
	T operator << (unsigned char value)
	{
		return f_value << value;
	}
	T operator << (int16_t value)
	{
		return f_value << value;
	}
	T operator << (uint16_t value)
	{
		return f_value << value;
	}
	T operator << (int32_t value)
	{
		return f_value << value;
	}
	T operator << (uint32_t value)
	{
		return f_value << value;
	}
#if UINT_MAX == ULONG_MAX
	T operator << (long value)
	{
		return f_value << value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator << (unsigned long value)
	{
		return f_value << value;
	}
#endif
	T operator << (int64_t value)
	{
		return f_value << value;
	}
	T operator << (uint64_t value)
	{
		return f_value << value;
	}
	T operator << (float value)
	{
		return f_value << value;
	}
	T operator << (double value)
	{
		return f_value << value;
	}
	T operator << (long double value)
	{
		return f_value << value;
	}
	T operator >> (const fauto_init& n)
	{
		return f_value >> n.f_value;
	}
	T operator >> (char value)
	{
		return f_value >> value;
	}
	T operator >> (signed char value)
	{
		return f_value >> value;
	}
	T operator >> (unsigned char value)
	{
		return f_value >> value;
	}
	T operator >> (int16_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint16_t value)
	{
		return f_value >> value;
	}
	T operator >> (int32_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint32_t value)
	{
		return f_value >> value;
	}
#if UINT_MAX == ULONG_MAX
	T operator >> (long value)
	{
		return f_value >> value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator >> (unsigned long value)
	{
		return f_value >> value;
	}
#endif
	T operator >> (int64_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint64_t value)
	{
		return f_value >> value;
	}
	T operator >> (float value)
	{
		return f_value >> value;
	}
	T operator >> (double value)
	{
		return f_value >> value;
	}
	T operator >> (long double value)
	{
		return f_value >> value;
	}
	T operator & (const fauto_init& n)
	{
		return f_value & n.f_value;
	}
	T operator & (char value)
	{
		return f_value & value;
	}
	T operator & (signed char value)
	{
		return f_value & value;
	}
	T operator & (unsigned char value)
	{
		return f_value & value;
	}
	T operator & (int16_t value)
	{
		return f_value & value;
	}
	T operator & (uint16_t value)
	{
		return f_value & value;
	}
	T operator & (int32_t value)
	{
		return f_value & value;
	}
	T operator & (uint32_t value)
	{
		return f_value & value;
	}
#if UINT_MAX == ULONG_MAX
	T operator & (long value)
	{
		return f_value & value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator & (unsigned long value)
	{
		return f_value & value;
	}
#endif
	T operator & (int64_t value)
	{
		return f_value & value;
	}
	T operator & (uint64_t value)
	{
		return f_value & value;
	}
	T operator & (float value)
	{
		return f_value & value;
	}
	T operator & (double value)
	{
		return f_value & value;
	}
	T operator & (long double value)
	{
		return f_value & value;
	}
	T operator | (const fauto_init& n)
	{
		return f_value | n.f_value;
	}
	T operator | (char value)
	{
		return f_value | value;
	}
	T operator | (signed char value)
	{
		return f_value | value;
	}
	T operator | (unsigned char value)
	{
		return f_value | value;
	}
	T operator | (int16_t value)
	{
		return f_value | value;
	}
	T operator | (uint16_t value)
	{
		return f_value | value;
	}
	T operator | (int32_t value)
	{
		return f_value | value;
	}
	T operator | (uint32_t value)
	{
		return f_value | value;
	}
#if UINT_MAX == ULONG_MAX
	T operator | (long value)
	{
		return f_value | value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator | (unsigned long value)
	{
		return f_value | value;
	}
#endif
	T operator | (int64_t value)
	{
		return f_value | value;
	}
	T operator | (uint64_t value)
	{
		return f_value | value;
	}
	T operator | (float value)
	{
		return f_value | value;
	}
	T operator | (double value)
	{
		return f_value | value;
	}
	T operator | (long double value)
	{
		return f_value | value;
	}
	T operator ^ (const fauto_init& n)
	{
		return f_value ^ n.f_value;
	}
	T operator ^ (char value)
	{
		return f_value ^ value;
	}
	T operator ^ (signed char value)
	{
		return f_value ^ value;
	}
	T operator ^ (unsigned char value)
	{
		return f_value ^ value;
	}
	T operator ^ (int16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (int32_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint32_t value)
	{
		return f_value ^ value;
	}
#if UINT_MAX == ULONG_MAX
	T operator ^ (long value)
	{
		return f_value ^ value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator ^ (unsigned long value)
	{
		return f_value ^ value;
	}
#endif
	T operator ^ (int64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (float value)
	{
		return f_value ^ value;
	}
	T operator ^ (double value)
	{
		return f_value ^ value;
	}
	T operator ^ (long double value)
	{
		return f_value ^ value;
	}
	bool operator == (const fauto_init& n)
	{
		return f_value == n.f_value;
	}
	bool operator == (char value)
	{
		return f_value == value;
	}
	bool operator == (signed char value)
	{
		return f_value == value;
	}
	bool operator == (unsigned char value)
	{
		return f_value == value;
	}
	bool operator == (int16_t value)
	{
		return f_value == value;
	}
	bool operator == (uint16_t value)
	{
		return f_value == value;
	}
	bool operator == (int32_t value)
	{
		return f_value == value;
	}
	bool operator == (uint32_t value)
	{
		return f_value == value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator == (long value)
	{
		return f_value == value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator == (unsigned long value)
	{
		return f_value == value;
	}
#endif
	bool operator == (int64_t value)
	{
		return f_value == value;
	}
	bool operator == (uint64_t value)
	{
		return f_value == value;
	}
	bool operator == (float value)
	{
		return f_value == value;
	}
	bool operator == (double value)
	{
		return f_value == value;
	}
	bool operator == (long double value)
	{
		return f_value == value;
	}
	bool operator != (const fauto_init& n)
	{
		return f_value != n.f_value;
	}
	bool operator != (char value)
	{
		return f_value != value;
	}
	bool operator != (signed char value)
	{
		return f_value != value;
	}
	bool operator != (unsigned char value)
	{
		return f_value != value;
	}
	bool operator != (int16_t value)
	{
		return f_value != value;
	}
	bool operator != (uint16_t value)
	{
		return f_value != value;
	}
	bool operator != (int32_t value)
	{
		return f_value != value;
	}
	bool operator != (uint32_t value)
	{
		return f_value != value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator != (long value)
	{
		return f_value != value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator != (unsigned long value)
	{
		return f_value != value;
	}
#endif
	bool operator != (int64_t value)
	{
		return f_value != value;
	}
	bool operator != (uint64_t value)
	{
		return f_value != value;
	}
	bool operator != (float value)
	{
		return f_value != value;
	}
	bool operator != (double value)
	{
		return f_value != value;
	}
	bool operator != (long double value)
	{
		return f_value != value;
	}
	bool operator < (const fauto_init& n)
	{
		return f_value < n.f_value;
	}
	bool operator < (char value)
	{
		return f_value < value;
	}
	bool operator < (signed char value)
	{
		return f_value < value;
	}
	bool operator < (unsigned char value)
	{
		return f_value < value;
	}
	bool operator < (int16_t value)
	{
		return f_value < value;
	}
	bool operator < (uint16_t value)
	{
		return f_value < value;
	}
	bool operator < (int32_t value)
	{
		return f_value < value;
	}
	bool operator < (uint32_t value)
	{
		return f_value < value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator < (long value)
	{
		return f_value < value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator < (unsigned long value)
	{
		return f_value < value;
	}
#endif
	bool operator < (int64_t value)
	{
		return f_value < value;
	}
	bool operator < (uint64_t value)
	{
		return f_value < value;
	}
	bool operator < (float value)
	{
		return f_value < value;
	}
	bool operator < (double value)
	{
		return f_value < value;
	}
	bool operator < (long double value)
	{
		return f_value < value;
	}
	bool operator <= (const fauto_init& n)
	{
		return f_value <= n.f_value;
	}
	bool operator <= (char value)
	{
		return f_value <= value;
	}
	bool operator <= (signed char value)
	{
		return f_value <= value;
	}
	bool operator <= (unsigned char value)
	{
		return f_value <= value;
	}
	bool operator <= (int16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (int32_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint32_t value)
	{
		return f_value <= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator <= (long value)
	{
		return f_value <= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator <= (unsigned long value)
	{
		return f_value <= value;
	}
#endif
	bool operator <= (int64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (float value)
	{
		return f_value <= value;
	}
	bool operator <= (double value)
	{
		return f_value <= value;
	}
	bool operator <= (long double value)
	{
		return f_value <= value;
	}
	bool operator > (const fauto_init& n)
	{
		return f_value > n.f_value;
	}
	bool operator > (char value)
	{
		return f_value > value;
	}
	bool operator > (signed char value)
	{
		return f_value > value;
	}
	bool operator > (unsigned char value)
	{
		return f_value > value;
	}
	bool operator > (int16_t value)
	{
		return f_value > value;
	}
	bool operator > (uint16_t value)
	{
		return f_value > value;
	}
	bool operator > (int32_t value)
	{
		return f_value > value;
	}
	bool operator > (uint32_t value)
	{
		return f_value > value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator > (long value)
	{
		return f_value > value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator > (unsigned long value)
	{
		return f_value > value;
	}
#endif
	bool operator > (int64_t value)
	{
		return f_value > value;
	}
	bool operator > (uint64_t value)
	{
		return f_value > value;
	}
	bool operator > (float value)
	{
		return f_value > value;
	}
	bool operator > (double value)
	{
		return f_value > value;
	}
	bool operator > (long double value)
	{
		return f_value > value;
	}
	bool operator >= (const fauto_init& n)
	{
		return f_value >= n.f_value;
	}
	bool operator >= (char value)
	{
		return f_value >= value;
	}
	bool operator >= (signed char value)
	{
		return f_value >= value;
	}
	bool operator >= (unsigned char value)
	{
		return f_value >= value;
	}
	bool operator >= (int16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (int32_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint32_t value)
	{
		return f_value >= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator >= (long value)
	{
		return f_value >= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator >= (unsigned long value)
	{
		return f_value >= value;
	}
#endif
	bool operator >= (int64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (float value)
	{
		return f_value >= value;
	}
	bool operator >= (double value)
	{
		return f_value >= value;
	}
	bool operator >= (long double value)
	{
		return f_value >= value;
	}

#ifdef MO_DEBUG
	bool is_initialized() const
	{
		return true;
	}
#endif

private:
	T f_value;
};

typedef fauto_init<float>	zfloat_t;
typedef fauto_init<double>	zdouble_t;
typedef fauto_init<long double>	zlongdouble_t;

template<class T>
class need_init
{
public:
	typedef T	primary_type_t;
	need_init(char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(signed char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(unsigned char value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(int16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(uint16_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(int32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(uint32_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#if UINT_MAX == ULONG_MAX
	need_init(long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init(unsigned long value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
#endif
	need_init(int64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(uint64_t value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(float value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	need_init(long double value)
	{
		f_value = static_cast<primary_type_t>(value);
	}
	operator T () const
	{
		return f_value;
	}
	operator T ()
	{
		return f_value;
	}
	const T * Ptr() const
	{
		return &f_value;
	}
	 T * Ptr() 
	{
		return &f_value;
	}
	bool operator ! () const
	{
		return !f_value;
	}
	T operator ~ () const
	{
		return ~f_value;
	}
	T operator + () const
	{
		return +f_value;
	}
	T operator - () const
	{
		return -f_value;
	}
	need_init& operator ++ (void)
	{
		++f_value;
		return *this;
	}
	need_init operator ++ (int)
	{
		need_init<T> result(*this);
		++f_value;
		return result;
	}
	need_init& operator -- (void)
	{
		--f_value;
		return *this;
	}
	need_init operator -- (int)
	{
		need_init<T> result(*this);
		--f_value;
		return result;
	}
	need_init& operator = (const need_init& n)
	{
		f_value = n.f_value;
		return *this;
	}
	need_init& operator = (char value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (signed char value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (unsigned char value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (int16_t value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (uint16_t value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (int32_t value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (uint32_t value)
	{
		f_value = value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator = (long value)
	{
		f_value = value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator = (unsigned long value)
	{
		f_value = value;
		return *this;
	}
#endif
	need_init& operator = (int64_t value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (uint64_t value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (float value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (double value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator = (long double value)
	{
		f_value = value;
		return *this;
	}
	need_init& operator *= (const need_init& n)
	{
		f_value *= n.f_value;
		return *this;
	}
	need_init& operator *= (char value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (signed char value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (unsigned char value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (int16_t value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (uint16_t value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (int32_t value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (uint32_t value)
	{
		f_value *= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator *= (long value)
	{
		f_value *= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator *= (unsigned long value)
	{
		f_value *= value;
		return *this;
	}
#endif
	need_init& operator *= (int64_t value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (uint64_t value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (float value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (double value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator *= (long double value)
	{
		f_value *= value;
		return *this;
	}
	need_init& operator /= (const need_init& n)
	{
		f_value /= n.f_value;
		return *this;
	}
	need_init& operator /= (char value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (signed char value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (unsigned char value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (int16_t value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (uint16_t value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (int32_t value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (uint32_t value)
	{
		f_value /= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator /= (long value)
	{
		f_value /= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator /= (unsigned long value)
	{
		f_value /= value;
		return *this;
	}
#endif
	need_init& operator /= (int64_t value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (uint64_t value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (float value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (double value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator /= (long double value)
	{
		f_value /= value;
		return *this;
	}
	need_init& operator %= (const need_init& n)
	{
		f_value %= n.f_value;
		return *this;
	}
	need_init& operator %= (char value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (signed char value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (unsigned char value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (int16_t value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (uint16_t value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (int32_t value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (uint32_t value)
	{
		f_value %= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator %= (long value)
	{
		f_value %= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator %= (unsigned long value)
	{
		f_value %= value;
		return *this;
	}
#endif
	need_init& operator %= (int64_t value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (uint64_t value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (float value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (double value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator %= (long double value)
	{
		f_value %= value;
		return *this;
	}
	need_init& operator += (const need_init& n)
	{
		f_value += n.f_value;
		return *this;
	}
	need_init& operator += (char value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (signed char value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (unsigned char value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (int16_t value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (uint16_t value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (int32_t value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (uint32_t value)
	{
		f_value += value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator += (long value)
	{
		f_value += value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator += (unsigned long value)
	{
		f_value += value;
		return *this;
	}
#endif
	need_init& operator += (int64_t value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (uint64_t value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (float value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (double value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator += (long double value)
	{
		f_value += value;
		return *this;
	}
	need_init& operator -= (const need_init& n)
	{
		f_value -= n.f_value;
		return *this;
	}
	need_init& operator -= (char value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (signed char value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (unsigned char value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (int16_t value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (uint16_t value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (int32_t value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (uint32_t value)
	{
		f_value -= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator -= (long value)
	{
		f_value -= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator -= (unsigned long value)
	{
		f_value -= value;
		return *this;
	}
#endif
	need_init& operator -= (int64_t value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (uint64_t value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (float value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (double value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator -= (long double value)
	{
		f_value -= value;
		return *this;
	}
	need_init& operator <<= (const need_init& n)
	{
		f_value <<= n.f_value;
		return *this;
	}
	need_init& operator <<= (char value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (signed char value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (unsigned char value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (int16_t value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (uint16_t value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (int32_t value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (uint32_t value)
	{
		f_value <<= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator <<= (long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator <<= (unsigned long value)
	{
		f_value <<= value;
		return *this;
	}
#endif
	need_init& operator <<= (int64_t value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (uint64_t value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (float value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (double value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator <<= (long double value)
	{
		f_value <<= value;
		return *this;
	}
	need_init& operator >>= (const need_init& n)
	{
		f_value >>= n.f_value;
		return *this;
	}
	need_init& operator >>= (char value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (signed char value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (unsigned char value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (int16_t value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (uint16_t value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (int32_t value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (uint32_t value)
	{
		f_value >>= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator >>= (long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator >>= (unsigned long value)
	{
		f_value >>= value;
		return *this;
	}
#endif
	need_init& operator >>= (int64_t value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (uint64_t value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (float value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (double value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator >>= (long double value)
	{
		f_value >>= value;
		return *this;
	}
	need_init& operator &= (const need_init& n)
	{
		f_value &= n.f_value;
		return *this;
	}
	need_init& operator &= (char value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (signed char value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (unsigned char value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (int16_t value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (uint16_t value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (int32_t value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (uint32_t value)
	{
		f_value &= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator &= (long value)
	{
		f_value &= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator &= (unsigned long value)
	{
		f_value &= value;
		return *this;
	}
#endif
	need_init& operator &= (int64_t value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (uint64_t value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (float value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (double value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator &= (long double value)
	{
		f_value &= value;
		return *this;
	}
	need_init& operator |= (const need_init& n)
	{
		f_value |= n.f_value;
		return *this;
	}
	need_init& operator |= (char value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (signed char value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (unsigned char value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (int16_t value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (uint16_t value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (int32_t value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (uint32_t value)
	{
		f_value |= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator |= (long value)
	{
		f_value |= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator |= (unsigned long value)
	{
		f_value |= value;
		return *this;
	}
#endif
	need_init& operator |= (int64_t value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (uint64_t value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (float value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (double value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator |= (long double value)
	{
		f_value |= value;
		return *this;
	}
	need_init& operator ^= (const need_init& n)
	{
		f_value ^= n.f_value;
		return *this;
	}
	need_init& operator ^= (char value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (signed char value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (unsigned char value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (int16_t value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (uint16_t value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (int32_t value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (uint32_t value)
	{
		f_value ^= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	need_init& operator ^= (long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	need_init& operator ^= (unsigned long value)
	{
		f_value ^= value;
		return *this;
	}
#endif
	need_init& operator ^= (int64_t value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (uint64_t value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (float value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (double value)
	{
		f_value ^= value;
		return *this;
	}
	need_init& operator ^= (long double value)
	{
		f_value ^= value;
		return *this;
	}
	T operator * (const need_init& n)
	{
		return f_value * n.f_value;
	}
	T operator * (char value)
	{
		return f_value * value;
	}
	T operator * (signed char value)
	{
		return f_value * value;
	}
	T operator * (unsigned char value)
	{
		return f_value * value;
	}
	T operator * (int16_t value)
	{
		return f_value * value;
	}
	T operator * (uint16_t value)
	{
		return f_value * value;
	}
	T operator * (int32_t value)
	{
		return f_value * value;
	}
	T operator * (uint32_t value)
	{
		return f_value * value;
	}
#if UINT_MAX == ULONG_MAX
	T operator * (long value)
	{
		return f_value * value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator * (unsigned long value)
	{
		return f_value * value;
	}
#endif
	T operator * (int64_t value)
	{
		return f_value * value;
	}
	T operator * (uint64_t value)
	{
		return f_value * value;
	}
	T operator * (float value)
	{
		return f_value * value;
	}
	T operator * (double value)
	{
		return f_value * value;
	}
	T operator * (long double value)
	{
		return f_value * value;
	}
	T operator / (const need_init& n)
	{
		return f_value / n.f_value;
	}
	T operator / (char value)
	{
		return f_value / value;
	}
	T operator / (signed char value)
	{
		return f_value / value;
	}
	T operator / (unsigned char value)
	{
		return f_value / value;
	}
	T operator / (int16_t value)
	{
		return f_value / value;
	}
	T operator / (uint16_t value)
	{
		return f_value / value;
	}
	T operator / (int32_t value)
	{
		return f_value / value;
	}
	T operator / (uint32_t value)
	{
		return f_value / value;
	}
#if UINT_MAX == ULONG_MAX
	T operator / (long value)
	{
		return f_value / value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator / (unsigned long value)
	{
		return f_value / value;
	}
#endif
	T operator / (int64_t value)
	{
		return f_value / value;
	}
	T operator / (uint64_t value)
	{
		return f_value / value;
	}
	T operator / (float value)
	{
		return f_value / value;
	}
	T operator / (double value)
	{
		return f_value / value;
	}
	T operator / (long double value)
	{
		return f_value / value;
	}
	T operator % (const need_init& n)
	{
		return f_value % n.f_value;
	}
	T operator % (char value)
	{
		return f_value % value;
	}
	T operator % (signed char value)
	{
		return f_value % value;
	}
	T operator % (unsigned char value)
	{
		return f_value % value;
	}
	T operator % (int16_t value)
	{
		return f_value % value;
	}
	T operator % (uint16_t value)
	{
		return f_value % value;
	}
	T operator % (int32_t value)
	{
		return f_value % value;
	}
	T operator % (uint32_t value)
	{
		return f_value % value;
	}
#if UINT_MAX == ULONG_MAX
	T operator % (long value)
	{
		return f_value % value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator % (unsigned long value)
	{
		return f_value % value;
	}
#endif
	T operator % (int64_t value)
	{
		return f_value % value;
	}
	T operator % (uint64_t value)
	{
		return f_value % value;
	}
	T operator % (float value)
	{
		return f_value % value;
	}
	T operator % (double value)
	{
		return f_value % value;
	}
	T operator % (long double value)
	{
		return f_value % value;
	}
	T operator + (const need_init& n)
	{
		return f_value + n.f_value;
	}
	T operator + (char value)
	{
		return f_value + value;
	}
	T operator + (signed char value)
	{
		return f_value + value;
	}
	T operator + (unsigned char value)
	{
		return f_value + value;
	}
	T operator + (int16_t value)
	{
		return f_value + value;
	}
	T operator + (uint16_t value)
	{
		return f_value + value;
	}
	T operator + (int32_t value)
	{
		return f_value + value;
	}
	T operator + (uint32_t value)
	{
		return f_value + value;
	}
#if UINT_MAX == ULONG_MAX
	T operator + (long value)
	{
		return f_value + value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator + (unsigned long value)
	{
		return f_value + value;
	}
#endif
	T operator + (int64_t value)
	{
		return f_value + value;
	}
	T operator + (uint64_t value)
	{
		return f_value + value;
	}
	T operator + (float value)
	{
		return f_value + value;
	}
	T operator + (double value)
	{
		return f_value + value;
	}
	T operator + (long double value)
	{
		return f_value + value;
	}
	T operator - (const need_init& n)
	{
		return f_value - n.f_value;
	}
	T operator - (char value)
	{
		return f_value - value;
	}
	T operator - (signed char value)
	{
		return f_value - value;
	}
	T operator - (unsigned char value)
	{
		return f_value - value;
	}
	T operator - (int16_t value)
	{
		return f_value - value;
	}
	T operator - (uint16_t value)
	{
		return f_value - value;
	}
	T operator - (int32_t value)
	{
		return f_value - value;
	}
	T operator - (uint32_t value)
	{
		return f_value - value;
	}
#if UINT_MAX == ULONG_MAX
	T operator - (long value)
	{
		return f_value - value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator - (unsigned long value)
	{
		return f_value - value;
	}
#endif
	T operator - (int64_t value)
	{
		return f_value - value;
	}
	T operator - (uint64_t value)
	{
		return f_value - value;
	}
	T operator - (float value)
	{
		return f_value - value;
	}
	T operator - (double value)
	{
		return f_value - value;
	}
	T operator - (long double value)
	{
		return f_value - value;
	}
	T operator << (const need_init& n)
	{
		return f_value << n.f_value;
	}
	T operator << (char value)
	{
		return f_value << value;
	}
	T operator << (signed char value)
	{
		return f_value << value;
	}
	T operator << (unsigned char value)
	{
		return f_value << value;
	}
	T operator << (int16_t value)
	{
		return f_value << value;
	}
	T operator << (uint16_t value)
	{
		return f_value << value;
	}
	T operator << (int32_t value)
	{
		return f_value << value;
	}
	T operator << (uint32_t value)
	{
		return f_value << value;
	}
#if UINT_MAX == ULONG_MAX
	T operator << (long value)
	{
		return f_value << value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator << (unsigned long value)
	{
		return f_value << value;
	}
#endif
	T operator << (int64_t value)
	{
		return f_value << value;
	}
	T operator << (uint64_t value)
	{
		return f_value << value;
	}
	T operator << (float value)
	{
		return f_value << value;
	}
	T operator << (double value)
	{
		return f_value << value;
	}
	T operator << (long double value)
	{
		return f_value << value;
	}
	T operator >> (const need_init& n)
	{
		return f_value >> n.f_value;
	}
	T operator >> (char value)
	{
		return f_value >> value;
	}
	T operator >> (signed char value)
	{
		return f_value >> value;
	}
	T operator >> (unsigned char value)
	{
		return f_value >> value;
	}
	T operator >> (int16_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint16_t value)
	{
		return f_value >> value;
	}
	T operator >> (int32_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint32_t value)
	{
		return f_value >> value;
	}
#if UINT_MAX == ULONG_MAX
	T operator >> (long value)
	{
		return f_value >> value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator >> (unsigned long value)
	{
		return f_value >> value;
	}
#endif
	T operator >> (int64_t value)
	{
		return f_value >> value;
	}
	T operator >> (uint64_t value)
	{
		return f_value >> value;
	}
	T operator >> (float value)
	{
		return f_value >> value;
	}
	T operator >> (double value)
	{
		return f_value >> value;
	}
	T operator >> (long double value)
	{
		return f_value >> value;
	}
	T operator & (const need_init& n)
	{
		return f_value & n.f_value;
	}
	T operator & (char value)
	{
		return f_value & value;
	}
	T operator & (signed char value)
	{
		return f_value & value;
	}
	T operator & (unsigned char value)
	{
		return f_value & value;
	}
	T operator & (int16_t value)
	{
		return f_value & value;
	}
	T operator & (uint16_t value)
	{
		return f_value & value;
	}
	T operator & (int32_t value)
	{
		return f_value & value;
	}
	T operator & (uint32_t value)
	{
		return f_value & value;
	}
#if UINT_MAX == ULONG_MAX
	T operator & (long value)
	{
		return f_value & value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator & (unsigned long value)
	{
		return f_value & value;
	}
#endif
	T operator & (int64_t value)
	{
		return f_value & value;
	}
	T operator & (uint64_t value)
	{
		return f_value & value;
	}
	T operator & (float value)
	{
		return f_value & value;
	}
	T operator & (double value)
	{
		return f_value & value;
	}
	T operator & (long double value)
	{
		return f_value & value;
	}
	T operator | (const need_init& n)
	{
		return f_value | n.f_value;
	}
	T operator | (char value)
	{
		return f_value | value;
	}
	T operator | (signed char value)
	{
		return f_value | value;
	}
	T operator | (unsigned char value)
	{
		return f_value | value;
	}
	T operator | (int16_t value)
	{
		return f_value | value;
	}
	T operator | (uint16_t value)
	{
		return f_value | value;
	}
	T operator | (int32_t value)
	{
		return f_value | value;
	}
	T operator | (uint32_t value)
	{
		return f_value | value;
	}
#if UINT_MAX == ULONG_MAX
	T operator | (long value)
	{
		return f_value | value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator | (unsigned long value)
	{
		return f_value | value;
	}
#endif
	T operator | (int64_t value)
	{
		return f_value | value;
	}
	T operator | (uint64_t value)
	{
		return f_value | value;
	}
	T operator | (float value)
	{
		return f_value | value;
	}
	T operator | (double value)
	{
		return f_value | value;
	}
	T operator | (long double value)
	{
		return f_value | value;
	}
	T operator ^ (const need_init& n)
	{
		return f_value ^ n.f_value;
	}
	T operator ^ (char value)
	{
		return f_value ^ value;
	}
	T operator ^ (signed char value)
	{
		return f_value ^ value;
	}
	T operator ^ (unsigned char value)
	{
		return f_value ^ value;
	}
	T operator ^ (int16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint16_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (int32_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint32_t value)
	{
		return f_value ^ value;
	}
#if UINT_MAX == ULONG_MAX
	T operator ^ (long value)
	{
		return f_value ^ value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator ^ (unsigned long value)
	{
		return f_value ^ value;
	}
#endif
	T operator ^ (int64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (uint64_t value)
	{
		return f_value ^ value;
	}
	T operator ^ (float value)
	{
		return f_value ^ value;
	}
	T operator ^ (double value)
	{
		return f_value ^ value;
	}
	T operator ^ (long double value)
	{
		return f_value ^ value;
	}
	bool operator == (const need_init& n)
	{
		return f_value == n.f_value;
	}
	bool operator == (char value)
	{
		return f_value == value;
	}
	bool operator == (signed char value)
	{
		return f_value == value;
	}
	bool operator == (unsigned char value)
	{
		return f_value == value;
	}
	bool operator == (int16_t value)
	{
		return f_value == value;
	}
	bool operator == (uint16_t value)
	{
		return f_value == value;
	}
	bool operator == (int32_t value)
	{
		return f_value == value;
	}
	bool operator == (uint32_t value)
	{
		return f_value == value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator == (long value)
	{
		return f_value == value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator == (unsigned long value)
	{
		return f_value == value;
	}
#endif
	bool operator == (int64_t value)
	{
		return f_value == value;
	}
	bool operator == (uint64_t value)
	{
		return f_value == value;
	}
	bool operator == (float value)
	{
		return f_value == value;
	}
	bool operator == (double value)
	{
		return f_value == value;
	}
	bool operator == (long double value)
	{
		return f_value == value;
	}
	bool operator != (const need_init& n)
	{
		return f_value != n.f_value;
	}
	bool operator != (char value)
	{
		return f_value != value;
	}
	bool operator != (signed char value)
	{
		return f_value != value;
	}
	bool operator != (unsigned char value)
	{
		return f_value != value;
	}
	bool operator != (int16_t value)
	{
		return f_value != value;
	}
	bool operator != (uint16_t value)
	{
		return f_value != value;
	}
	bool operator != (int32_t value)
	{
		return f_value != value;
	}
	bool operator != (uint32_t value)
	{
		return f_value != value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator != (long value)
	{
		return f_value != value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator != (unsigned long value)
	{
		return f_value != value;
	}
#endif
	bool operator != (int64_t value)
	{
		return f_value != value;
	}
	bool operator != (uint64_t value)
	{
		return f_value != value;
	}
	bool operator != (float value)
	{
		return f_value != value;
	}
	bool operator != (double value)
	{
		return f_value != value;
	}
	bool operator != (long double value)
	{
		return f_value != value;
	}
	bool operator < (const need_init& n)
	{
		return f_value < n.f_value;
	}
	bool operator < (char value)
	{
		return f_value < value;
	}
	bool operator < (signed char value)
	{
		return f_value < value;
	}
	bool operator < (unsigned char value)
	{
		return f_value < value;
	}
	bool operator < (int16_t value)
	{
		return f_value < value;
	}
	bool operator < (uint16_t value)
	{
		return f_value < value;
	}
	bool operator < (int32_t value)
	{
		return f_value < value;
	}
	bool operator < (uint32_t value)
	{
		return f_value < value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator < (long value)
	{
		return f_value < value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator < (unsigned long value)
	{
		return f_value < value;
	}
#endif
	bool operator < (int64_t value)
	{
		return f_value < value;
	}
	bool operator < (uint64_t value)
	{
		return f_value < value;
	}
	bool operator < (float value)
	{
		return f_value < value;
	}
	bool operator < (double value)
	{
		return f_value < value;
	}
	bool operator < (long double value)
	{
		return f_value < value;
	}
	bool operator <= (const need_init& n)
	{
		return f_value <= n.f_value;
	}
	bool operator <= (char value)
	{
		return f_value <= value;
	}
	bool operator <= (signed char value)
	{
		return f_value <= value;
	}
	bool operator <= (unsigned char value)
	{
		return f_value <= value;
	}
	bool operator <= (int16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint16_t value)
	{
		return f_value <= value;
	}
	bool operator <= (int32_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint32_t value)
	{
		return f_value <= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator <= (long value)
	{
		return f_value <= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator <= (unsigned long value)
	{
		return f_value <= value;
	}
#endif
	bool operator <= (int64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (uint64_t value)
	{
		return f_value <= value;
	}
	bool operator <= (float value)
	{
		return f_value <= value;
	}
	bool operator <= (double value)
	{
		return f_value <= value;
	}
	bool operator <= (long double value)
	{
		return f_value <= value;
	}
	bool operator > (const need_init& n)
	{
		return f_value > n.f_value;
	}
	bool operator > (char value)
	{
		return f_value > value;
	}
	bool operator > (signed char value)
	{
		return f_value > value;
	}
	bool operator > (unsigned char value)
	{
		return f_value > value;
	}
	bool operator > (int16_t value)
	{
		return f_value > value;
	}
	bool operator > (uint16_t value)
	{
		return f_value > value;
	}
	bool operator > (int32_t value)
	{
		return f_value > value;
	}
	bool operator > (uint32_t value)
	{
		return f_value > value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator > (long value)
	{
		return f_value > value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator > (unsigned long value)
	{
		return f_value > value;
	}
#endif
	bool operator > (int64_t value)
	{
		return f_value > value;
	}
	bool operator > (uint64_t value)
	{
		return f_value > value;
	}
	bool operator > (float value)
	{
		return f_value > value;
	}
	bool operator > (double value)
	{
		return f_value > value;
	}
	bool operator > (long double value)
	{
		return f_value > value;
	}
	bool operator >= (const need_init& n)
	{
		return f_value >= n.f_value;
	}
	bool operator >= (char value)
	{
		return f_value >= value;
	}
	bool operator >= (signed char value)
	{
		return f_value >= value;
	}
	bool operator >= (unsigned char value)
	{
		return f_value >= value;
	}
	bool operator >= (int16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint16_t value)
	{
		return f_value >= value;
	}
	bool operator >= (int32_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint32_t value)
	{
		return f_value >= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator >= (long value)
	{
		return f_value >= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator >= (unsigned long value)
	{
		return f_value >= value;
	}
#endif
	bool operator >= (int64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (uint64_t value)
	{
		return f_value >= value;
	}
	bool operator >= (float value)
	{
		return f_value >= value;
	}
	bool operator >= (double value)
	{
		return f_value >= value;
	}
	bool operator >= (long double value)
	{
		return f_value >= value;
	}

#ifdef MO_DEBUG
	bool is_initialized() const
	{
		return true;
	}
#endif

private:
	T f_value;
};

typedef need_init<bool>	mbool_t;
typedef need_init<char>	mchar_t;
typedef need_init<signed char>	mschar_t;
typedef need_init<unsigned char>	muchar_t;
typedef need_init<int16_t>	mint16_t;
typedef need_init<uint16_t>	muint16_t;
typedef need_init<int32_t>	mint32_t;
typedef need_init<uint32_t>	muint32_t;
#if UINT_MAX == ULONG_MAX
typedef need_init<long>	mplain_long_t;
#endif
#if UINT_MAX == ULONG_MAX
typedef need_init<unsigned long>	mplain_ulong_t;
#endif
typedef need_init<int64_t>	mint64_t;
typedef need_init<uint64_t>	muint64_t;
typedef need_init<float>	mfloat_t;
typedef need_init<double>	mdouble_t;
typedef need_init<long double>	mlongdouble_t;
typedef need_init<size_t>	msize_t;
typedef need_init<time_t>	mtime_t;

#ifdef MO_DEBUG
template<class T>
class no_init
{
public:
	typedef T	primary_type_t;
	no_init()
	{
		f_initialized = false;
	}
	no_init(char value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(signed char value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(unsigned char value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(int16_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(uint16_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(int32_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(uint32_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
#if UINT_MAX == ULONG_MAX
	no_init(long value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init(unsigned long value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
#endif
	no_init(int64_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(uint64_t value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(float value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(double value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	no_init(long double value)
	{
		f_initialized = true;
		f_value = static_cast<primary_type_t>(value);
	}
	operator T () const
	{
		assert(f_initialized);
		return f_value;
	}
	operator T ()
	{
		assert(f_initialized);
		return f_value;
	}
	const T * Ptr() const
	{
		assert(f_initialized);
		return &f_value;
	}
	 T * Ptr() 
	{
		assert(f_initialized);
		return &f_value;
	}
	bool operator ! () const
	{
		assert(f_initialized);
		return !f_value;
	}
	T operator ~ () const
	{
		assert(f_initialized);
		return ~f_value;
	}
	T operator + () const
	{
		assert(f_initialized);
		return +f_value;
	}
	T operator - () const
	{
		assert(f_initialized);
		return -f_value;
	}
	no_init& operator ++ (void)
	{
		assert(f_initialized);
		++f_value;
		return *this;
	}
	no_init operator ++ (int)
	{
		assert(f_initialized);
		no_init<T> result(*this);
		++f_value;
		return result;
	}
	no_init& operator -- (void)
	{
		assert(f_initialized);
		--f_value;
		return *this;
	}
	no_init operator -- (int)
	{
		assert(f_initialized);
		no_init<T> result(*this);
		--f_value;
		return result;
	}
	no_init& operator = (const no_init& n)
	{
		f_initialized = true;
		assert(n.f_initialized);
		f_value = n.f_value;
		return *this;
	}
	no_init& operator = (char value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (signed char value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (unsigned char value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (int16_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (uint16_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (int32_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (uint32_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator = (long value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator = (unsigned long value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
#endif
	no_init& operator = (int64_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (uint64_t value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (float value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (double value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator = (long double value)
	{
		f_initialized = true;
		f_value = value;
		return *this;
	}
	no_init& operator *= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value *= n.f_value;
		return *this;
	}
	no_init& operator *= (char value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (signed char value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (unsigned char value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (int16_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (uint16_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (int32_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (uint32_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator *= (long value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator *= (unsigned long value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
#endif
	no_init& operator *= (int64_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (uint64_t value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (float value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (double value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator *= (long double value)
	{
		assert(f_initialized);
		f_value *= value;
		return *this;
	}
	no_init& operator /= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value /= n.f_value;
		return *this;
	}
	no_init& operator /= (char value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (signed char value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (unsigned char value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (int16_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (uint16_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (int32_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (uint32_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator /= (long value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator /= (unsigned long value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
#endif
	no_init& operator /= (int64_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (uint64_t value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (float value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (double value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator /= (long double value)
	{
		assert(f_initialized);
		f_value /= value;
		return *this;
	}
	no_init& operator %= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value %= n.f_value;
		return *this;
	}
	no_init& operator %= (char value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (signed char value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (unsigned char value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (int16_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (uint16_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (int32_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (uint32_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator %= (long value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator %= (unsigned long value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
#endif
	no_init& operator %= (int64_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (uint64_t value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (float value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (double value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator %= (long double value)
	{
		assert(f_initialized);
		f_value %= value;
		return *this;
	}
	no_init& operator += (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value += n.f_value;
		return *this;
	}
	no_init& operator += (char value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (signed char value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (unsigned char value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (int16_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (uint16_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (int32_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (uint32_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator += (long value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator += (unsigned long value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
#endif
	no_init& operator += (int64_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (uint64_t value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (float value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (double value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator += (long double value)
	{
		assert(f_initialized);
		f_value += value;
		return *this;
	}
	no_init& operator -= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value -= n.f_value;
		return *this;
	}
	no_init& operator -= (char value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (signed char value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (unsigned char value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (int16_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (uint16_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (int32_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (uint32_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator -= (long value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator -= (unsigned long value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
#endif
	no_init& operator -= (int64_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (uint64_t value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (float value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (double value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator -= (long double value)
	{
		assert(f_initialized);
		f_value -= value;
		return *this;
	}
	no_init& operator <<= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value <<= n.f_value;
		return *this;
	}
	no_init& operator <<= (char value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (signed char value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (unsigned char value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (int16_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (uint16_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (int32_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (uint32_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator <<= (long value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator <<= (unsigned long value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
#endif
	no_init& operator <<= (int64_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (uint64_t value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (float value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (double value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator <<= (long double value)
	{
		assert(f_initialized);
		f_value <<= value;
		return *this;
	}
	no_init& operator >>= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value >>= n.f_value;
		return *this;
	}
	no_init& operator >>= (char value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (signed char value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (unsigned char value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (int16_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (uint16_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (int32_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (uint32_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator >>= (long value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator >>= (unsigned long value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
#endif
	no_init& operator >>= (int64_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (uint64_t value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (float value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (double value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator >>= (long double value)
	{
		assert(f_initialized);
		f_value >>= value;
		return *this;
	}
	no_init& operator &= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value &= n.f_value;
		return *this;
	}
	no_init& operator &= (char value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (signed char value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (unsigned char value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (int16_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (uint16_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (int32_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (uint32_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator &= (long value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator &= (unsigned long value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
#endif
	no_init& operator &= (int64_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (uint64_t value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (float value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (double value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator &= (long double value)
	{
		assert(f_initialized);
		f_value &= value;
		return *this;
	}
	no_init& operator |= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value |= n.f_value;
		return *this;
	}
	no_init& operator |= (char value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (signed char value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (unsigned char value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (int16_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (uint16_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (int32_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (uint32_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator |= (long value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator |= (unsigned long value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
#endif
	no_init& operator |= (int64_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (uint64_t value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (float value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (double value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator |= (long double value)
	{
		assert(f_initialized);
		f_value |= value;
		return *this;
	}
	no_init& operator ^= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		f_value ^= n.f_value;
		return *this;
	}
	no_init& operator ^= (char value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (signed char value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (unsigned char value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (int16_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (uint16_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (int32_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (uint32_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
#if UINT_MAX == ULONG_MAX
	no_init& operator ^= (long value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
#endif
#if UINT_MAX == ULONG_MAX
	no_init& operator ^= (unsigned long value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
#endif
	no_init& operator ^= (int64_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (uint64_t value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (float value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (double value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	no_init& operator ^= (long double value)
	{
		assert(f_initialized);
		f_value ^= value;
		return *this;
	}
	T operator * (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value * n.f_value;
	}
	T operator * (char value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (signed char value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (unsigned char value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (int16_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (uint16_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (int32_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (uint32_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
#if UINT_MAX == ULONG_MAX
	T operator * (long value)
	{
		assert(f_initialized);
		return f_value * value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator * (unsigned long value)
	{
		assert(f_initialized);
		return f_value * value;
	}
#endif
	T operator * (int64_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (uint64_t value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (float value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (double value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator * (long double value)
	{
		assert(f_initialized);
		return f_value * value;
	}
	T operator / (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value / n.f_value;
	}
	T operator / (char value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (signed char value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (unsigned char value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (int16_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (uint16_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (int32_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (uint32_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
#if UINT_MAX == ULONG_MAX
	T operator / (long value)
	{
		assert(f_initialized);
		return f_value / value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator / (unsigned long value)
	{
		assert(f_initialized);
		return f_value / value;
	}
#endif
	T operator / (int64_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (uint64_t value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (float value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (double value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator / (long double value)
	{
		assert(f_initialized);
		return f_value / value;
	}
	T operator % (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value % n.f_value;
	}
	T operator % (char value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (signed char value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (unsigned char value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (int16_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (uint16_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (int32_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (uint32_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
#if UINT_MAX == ULONG_MAX
	T operator % (long value)
	{
		assert(f_initialized);
		return f_value % value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator % (unsigned long value)
	{
		assert(f_initialized);
		return f_value % value;
	}
#endif
	T operator % (int64_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (uint64_t value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (float value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (double value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator % (long double value)
	{
		assert(f_initialized);
		return f_value % value;
	}
	T operator + (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value + n.f_value;
	}
	T operator + (char value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (signed char value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (unsigned char value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (int16_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (uint16_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (int32_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (uint32_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
#if UINT_MAX == ULONG_MAX
	T operator + (long value)
	{
		assert(f_initialized);
		return f_value + value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator + (unsigned long value)
	{
		assert(f_initialized);
		return f_value + value;
	}
#endif
	T operator + (int64_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (uint64_t value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (float value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (double value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator + (long double value)
	{
		assert(f_initialized);
		return f_value + value;
	}
	T operator - (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value - n.f_value;
	}
	T operator - (char value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (signed char value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (unsigned char value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (int16_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (uint16_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (int32_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (uint32_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
#if UINT_MAX == ULONG_MAX
	T operator - (long value)
	{
		assert(f_initialized);
		return f_value - value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator - (unsigned long value)
	{
		assert(f_initialized);
		return f_value - value;
	}
#endif
	T operator - (int64_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (uint64_t value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (float value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (double value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator - (long double value)
	{
		assert(f_initialized);
		return f_value - value;
	}
	T operator << (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value << n.f_value;
	}
	T operator << (char value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (signed char value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (unsigned char value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (int16_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (uint16_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (int32_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (uint32_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
#if UINT_MAX == ULONG_MAX
	T operator << (long value)
	{
		assert(f_initialized);
		return f_value << value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator << (unsigned long value)
	{
		assert(f_initialized);
		return f_value << value;
	}
#endif
	T operator << (int64_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (uint64_t value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (float value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (double value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator << (long double value)
	{
		assert(f_initialized);
		return f_value << value;
	}
	T operator >> (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value >> n.f_value;
	}
	T operator >> (char value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (signed char value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (unsigned char value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (int16_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (uint16_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (int32_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (uint32_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
#if UINT_MAX == ULONG_MAX
	T operator >> (long value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator >> (unsigned long value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
#endif
	T operator >> (int64_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (uint64_t value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (float value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (double value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator >> (long double value)
	{
		assert(f_initialized);
		return f_value >> value;
	}
	T operator & (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value & n.f_value;
	}
	T operator & (char value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (signed char value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (unsigned char value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (int16_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (uint16_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (int32_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (uint32_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
#if UINT_MAX == ULONG_MAX
	T operator & (long value)
	{
		assert(f_initialized);
		return f_value & value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator & (unsigned long value)
	{
		assert(f_initialized);
		return f_value & value;
	}
#endif
	T operator & (int64_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (uint64_t value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (float value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (double value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator & (long double value)
	{
		assert(f_initialized);
		return f_value & value;
	}
	T operator | (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value | n.f_value;
	}
	T operator | (char value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (signed char value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (unsigned char value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (int16_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (uint16_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (int32_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (uint32_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
#if UINT_MAX == ULONG_MAX
	T operator | (long value)
	{
		assert(f_initialized);
		return f_value | value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator | (unsigned long value)
	{
		assert(f_initialized);
		return f_value | value;
	}
#endif
	T operator | (int64_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (uint64_t value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (float value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (double value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator | (long double value)
	{
		assert(f_initialized);
		return f_value | value;
	}
	T operator ^ (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value ^ n.f_value;
	}
	T operator ^ (char value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (signed char value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (unsigned char value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (int16_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (uint16_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (int32_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (uint32_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
#if UINT_MAX == ULONG_MAX
	T operator ^ (long value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	T operator ^ (unsigned long value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
#endif
	T operator ^ (int64_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (uint64_t value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (float value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (double value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	T operator ^ (long double value)
	{
		assert(f_initialized);
		return f_value ^ value;
	}
	bool operator == (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value == n.f_value;
	}
	bool operator == (char value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (signed char value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (unsigned char value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (int16_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (uint16_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (int32_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (uint32_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator == (long value)
	{
		assert(f_initialized);
		return f_value == value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator == (unsigned long value)
	{
		assert(f_initialized);
		return f_value == value;
	}
#endif
	bool operator == (int64_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (uint64_t value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (float value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (double value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator == (long double value)
	{
		assert(f_initialized);
		return f_value == value;
	}
	bool operator != (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value != n.f_value;
	}
	bool operator != (char value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (signed char value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (unsigned char value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (int16_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (uint16_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (int32_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (uint32_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator != (long value)
	{
		assert(f_initialized);
		return f_value != value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator != (unsigned long value)
	{
		assert(f_initialized);
		return f_value != value;
	}
#endif
	bool operator != (int64_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (uint64_t value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (float value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (double value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator != (long double value)
	{
		assert(f_initialized);
		return f_value != value;
	}
	bool operator < (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value < n.f_value;
	}
	bool operator < (char value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (signed char value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (unsigned char value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (int16_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (uint16_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (int32_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (uint32_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator < (long value)
	{
		assert(f_initialized);
		return f_value < value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator < (unsigned long value)
	{
		assert(f_initialized);
		return f_value < value;
	}
#endif
	bool operator < (int64_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (uint64_t value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (float value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (double value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator < (long double value)
	{
		assert(f_initialized);
		return f_value < value;
	}
	bool operator <= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value <= n.f_value;
	}
	bool operator <= (char value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (signed char value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (unsigned char value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (int16_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (uint16_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (int32_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (uint32_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator <= (long value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator <= (unsigned long value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
#endif
	bool operator <= (int64_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (uint64_t value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (float value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (double value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator <= (long double value)
	{
		assert(f_initialized);
		return f_value <= value;
	}
	bool operator > (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value > n.f_value;
	}
	bool operator > (char value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (signed char value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (unsigned char value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (int16_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (uint16_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (int32_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (uint32_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator > (long value)
	{
		assert(f_initialized);
		return f_value > value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator > (unsigned long value)
	{
		assert(f_initialized);
		return f_value > value;
	}
#endif
	bool operator > (int64_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (uint64_t value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (float value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (double value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator > (long double value)
	{
		assert(f_initialized);
		return f_value > value;
	}
	bool operator >= (const no_init& n)
	{
		assert(f_initialized);
		assert(n.f_initialized);
		return f_value >= n.f_value;
	}
	bool operator >= (char value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (signed char value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (unsigned char value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (int16_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (uint16_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (int32_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (uint32_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
#if UINT_MAX == ULONG_MAX
	bool operator >= (long value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
#endif
#if UINT_MAX == ULONG_MAX
	bool operator >= (unsigned long value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
#endif
	bool operator >= (int64_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (uint64_t value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (float value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (double value)
	{
		assert(f_initialized);
		return f_value >= value;
	}
	bool operator >= (long double value)
	{
		assert(f_initialized);
		return f_value >= value;
	}

#ifdef MO_DEBUG
	bool is_initialized() const
	{
		return f_initialized;
	}
#endif

private:
	bool f_initialized;
	T f_value;
};

typedef no_init<bool>	rbool_t;
typedef no_init<char>	rchar_t;
typedef no_init<signed char>	rschar_t;
typedef no_init<unsigned char>	ruchar_t;
typedef no_init<int16_t>	rint16_t;
typedef no_init<uint16_t>	ruint16_t;
typedef no_init<int32_t>	rint32_t;
typedef no_init<uint32_t>	ruint32_t;
#if UINT_MAX == ULONG_MAX
typedef no_init<long>	rplain_long_t;
#endif
#if UINT_MAX == ULONG_MAX
typedef no_init<unsigned long>	rplain_ulong_t;
#endif
typedef no_init<int64_t>	rint64_t;
typedef no_init<uint64_t>	ruint64_t;
typedef no_init<float>	rfloat_t;
typedef no_init<double>	rdouble_t;
typedef no_init<long double>	rlongdouble_t;
typedef no_init<size_t>	rsize_t;
typedef no_init<time_t>	rtime_t;
#else
typedef bool	rbool_t;
typedef char	rchar_t;
typedef signed char	rschar_t;
typedef unsigned char	ruchar_t;
typedef int16_t	rint16_t;
typedef uint16_t	ruint16_t;
typedef int32_t	rint32_t;
typedef uint32_t	ruint32_t;
#if UINT_MAX == ULONG_MAX
typedef long	rplain_long_t;
#endif
#if UINT_MAX == ULONG_MAX
typedef unsigned long	rplain_ulong_t;
#endif
typedef int64_t	rint64_t;
typedef uint64_t	ruint64_t;
typedef float	rfloat_t;
typedef double	rdouble_t;
typedef long double	rlongdouble_t;
typedef size_t	rsize_t;
typedef time_t	rtime_t;
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif


