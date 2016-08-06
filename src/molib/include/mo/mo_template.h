//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
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



#ifndef MO_TEMPLATE_H
#define	MO_TEMPLATE_H

#include	"mo_base.h"



namespace molib
{


/************************************************************ DOC:

CLASS

	moBase

NAME

	moSwap16Bits - swap the bytes of a short value
	moSwap32Bits - swap the bytes of a long value
	moSwap64Bits - swap the bytes of a long long value
	moRead16Bits - read a short at any address
	moReadBE16Bits - read a big endian short value from memory
	moReadLE16Bits - read a little endian short value from memory
	moRead32Bits - read a long at any adress
	moReadBE32Bits - read a big endian long value from memory
	moReadLE32Bits - read a little endian long value from memory
	moWrite16Bits - write a short at any address
	moWriteBE16Bits - write a big endian short value in memory
	moWriteLE16Bits - write a little endian short value in memory
	moWrite32Bits - write a long at any address
	moWriteBE32Bits - write a big endian long value in memory
	moWriteLE32Bits - write a little endian long value in memory

SYNOPSIS

	inline short moSwap16Bits(int16_t s);
	inline unsigned short moSwap16Bits(uint16_t s);
	inline long moSwap32Bits(int32_t l);
	inline unsigned long moSwap32Bits(uint32_t l);
	inline long moSwap64Bits(int64_t ll);
	inline unsigned long moSwap64Bits(uint64_t ll);
	inline short moReadShort(const void *source);
	inline short moReadBEShort(const void *source);
	inline short moReadLEShort(const void *source);
	inline long moReadLong(const void *source);
	inline long moReadBELong(const void *source);
	inline long moReadLELong(const void *source);
	inline void moWriteShort(void *destination, short s);
	inline void moWriteBEShort(void *destination, short s);
	inline void moWriteLEShort(void *destination, short s);
	inline void moWriteLong(void *destination, long l);
	inline void moWriteBELong(void *destination, long l);
	inline void moWriteLELong(void *destination, long l);

PARAMETERS

	s - a short value (16 bits)
	l - a long value (32 bits)
	ll - a long long value (64 bits)
	source - a pointer where the value is read
	destination - a pointer where the value is written

DESCRIPTION

	The swap functions swap the bytes of a signed or
	unsigned short or a long word of data.

	This is typically used to inverse data from one
	endian to another (from big to little and vice
	versa).

	The Read & Write functions can be used to directly
	read a little or big endian value from memory to
	a short or a long value which can be used by the
	running machine. The Read & Write function without
	a specific endian (BE or LE) can be used to read
	an unaligned short or long value from anywhere
	in memory.

	The Read and Write can also be used with unaligned
	memory pointers.

RETURN VALUE

	the swapped value for the swap functions
	the value in machine code for the read functions
	nothing (void) for the write functions

SEE ALSO

	SwapShortFromLittleEndian, SwapShortFromBigEndian,
	SwapLongFromLittleEndian, SwapLongFromBigEndian

*/
MO_DLL_EXPORT_FUNC inline int16_t moSwap16Bits(int16_t s)
{
#if (defined(__i386) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	asm("xchgb %b0,%h0" : "=q" (s) : "0" (s));
	return s;
#else
	return (int16_t) (((uint16_t) s >> 8) | (s << 8));
#endif
}

MO_DLL_EXPORT_FUNC inline uint16_t moSwap16Bits(uint16_t s)
{
#if (defined(__i386) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	asm("xchgb %b0,%h0" : "=q" (s) : "0" (s));
	return s;
#else
	return (s >> 8) | (s << 8);
#endif
}

MO_DLL_EXPORT_FUNC inline int32_t moSwap32Bits(int32_t l)
{
#if (defined(__i386) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	asm("bswap %0" : "=r" (l) : "0" (l));
	return l;
#else
	return (int32_t) (((uint32_t) l >> 24) | (l << 24)
		| ((l & 0x00FF0000) >> 8) | ((l & 0x0000FF00) << 8));
#endif
}

MO_DLL_EXPORT_FUNC inline uint32_t moSwap32Bits(uint32_t l)
{
#if (defined(__i386) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	asm("bswap %0" : "=r" (l) : "0" (l));
	return l;
#else
	return (uint32_t) ((l >> 24) | (l << 24)
		| ((l & 0x00FF0000) >> 8) | ((l & 0x0000FF00) << 8));
#endif
}


MO_DLL_EXPORT_FUNC inline int64_t moSwap64Bits(int64_t ll)
{
#if (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	uint32_t H = (uint32_t) (ll >> 32);
	uint32_t L = (uint32_t) ll;
	asm("bswap %0" : "=r" (H) : "0" (H));
	asm("bswap %0" : "=r" (L) : "0" (L));
	return (uint64_t) H | ((uint64_t) L << 32);
#else
	char *a, c;
	a = (char *)&ll;
	c = a[0];
	a[0] = a[7];
	a[7] = c;
	c = a[1];
	a[1] = a[6];
	a[6] = c;
	c = a[2];
	a[2] = a[5];
	a[5] = c;
	c = a[3];
	a[3] = a[4];
	a[4] = c;
	return ll;
#endif
}

MO_DLL_EXPORT_FUNC inline uint64_t moSwap64Bits(uint64_t ll)
{
#if (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && defined(__GNUC__)
	uint32_t H = (uint32_t) (ll >> 32);
	uint32_t L = (uint32_t) ll;
	asm("bswap %0" : "=r" (H) : "0" (H));
	asm("bswap %0" : "=r" (L) : "0" (L));
	return (uint64_t) H | ((uint64_t) L << 32);
#else
	char *a, c;
	a = (char *)&ll;
	c = a[0];
	a[0] = a[7];
	a[7] = c;
	c = a[1];
	a[1] = a[6];
	a[6] = c;
	c = a[2];
	a[2] = a[5];
	a[5] = c;
	c = a[3];
	a[3] = a[4];
	a[4] = c;
	return ll;
#endif
}


MO_DLL_EXPORT_FUNC inline int16_t moReadBE16Bits(const void *source)
{
	// create an alias (should be transparent in assembly)
	const unsigned char *s = reinterpret_cast<const unsigned char *>(source);
	return (s[0] << 8) + s[1];
}


MO_DLL_EXPORT_FUNC inline int16_t moReadLE16Bits(const void *source)
{
	// create an alias (should be transparent in assembly)
	const unsigned char *s = reinterpret_cast<const unsigned char *>(source);
	return s[0] + (s[1] << 8);
}


MO_DLL_EXPORT_FUNC inline int16_t moRead16Bits(const void *source)
{
#if BYTE_ORDER == BIG_ENDIAN
	return moReadBE16Bits(source);
#else
	return moReadLE16Bits(source);
#endif
}


MO_DLL_EXPORT_FUNC inline int32_t moReadBE32Bits(const void *source)
{
	// create an alias (should be transparent in assembly)
	const unsigned char *s = reinterpret_cast<const unsigned char *>(source);
	return (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
}


MO_DLL_EXPORT_FUNC inline int32_t moReadLE32Bits(const void *source)
{
	// create an alias (should be transparent in assembly)
	const unsigned char *s = reinterpret_cast<const unsigned char *>(source);
	return (s[3] << 24) + (s[2] << 16) + (s[1] << 8) + s[0];
}


MO_DLL_EXPORT_FUNC inline int32_t moRead32Bits(const void *source)
{
#if BYTE_ORDER == BIG_ENDIAN
	return moReadBE32Bits(source);
#else
	return moReadLE32Bits(source);
#endif
}


MO_DLL_EXPORT_FUNC inline void moWriteBE16Bits(void *destination, int16_t s)
{
	// create an alias (should be transparent in assembly)
	unsigned char *d = reinterpret_cast<unsigned char *>(destination);
	d[0] = (unsigned char) (s >> 8);
	d[1] = (unsigned char) s;
}


MO_DLL_EXPORT_FUNC inline void moWriteLE16Bits(void *destination, int16_t s)
{
	// create an alias (should be transparent in assembly)
	unsigned char *d = reinterpret_cast<unsigned char *>(destination);
	d[1] = (unsigned char) (s >> 8);
	d[0] = (unsigned char) s;
}


MO_DLL_EXPORT_FUNC inline void moWrite16Bits(void *destination, int16_t l)
{
#if BYTE_ORDER == BIG_ENDIAN
	moWriteBE16Bits(destination, l);
#else
	moWriteLE16Bits(destination, l);
#endif
}


MO_DLL_EXPORT_FUNC inline void moWriteBE32Bits(void *destination, int32_t l)
{
	// create an alias (should be transparent in assembly)
	unsigned char *d = reinterpret_cast<unsigned char *>(destination);
	d[0] = l >> 24;
	d[1] = l >> 16;
	d[2] = l >> 8;
	d[3] = l;
}


MO_DLL_EXPORT_FUNC inline void moWriteLE32Bits(void *destination, int32_t l)
{
	// create an alias (should be transparent in assembly)
	unsigned char *d = reinterpret_cast<unsigned char *>(destination);
	d[3] = l >> 24;
	d[2] = l >> 16;
	d[1] = l >> 8;
	d[0] = l;
}


MO_DLL_EXPORT_FUNC inline void moWrite32Bits(void *destination, int32_t l)
{
#if BYTE_ORDER == BIG_ENDIAN
	moWriteBE32Bits(destination, l);
#else
	moWriteLE32Bits(destination, l);
#endif
}



/************************************************************ DOC:

CLASS

	None

NAME

	moMin - returns the minimum of two values
	moMax - returns the maximum of two values
	moRange - clamps a value between a minimum and maximum

SYNOPSIS

	template<class T1, class T2> T1 moMin(T1 a, T2 b);
	template<class T1, class T2, class T3> T1 moMin(T1 a, T2 b, T3 c);
	template<class T1, class T2> T1 moMax(T1 a, T2 b);
	template<class T1, class T2, class T3> T1 moMax(T1 a, T2 b, T3 c);

	template<class T1, class T2, class T3> T2 moClamp(T1 min, T2 value, T3 max);

PARAMETERS

	a - a value
	b - a value
	c - a value
	min - the minimum that value can be set at
	value - the value to be clamped
	max - the maximum that value can be set at

DESCRIPTION

	The moMin() and moMax() return the minimum and maximum of the two
	specified values.

	The moClamp() function returns the value clamped between min and max
	inclusive. It returns the minimum when value is smaller than min and
	the maximum when value if larger than max.

RETURN VALUE

	for moMin() the smallest value
	for moMax() the largest value
	for moClamp() min if value is smaller, max if value is larger otherwise value

BUGS

	moClamp() does not test the validity of the min and max values. It is
	expected that min <= max. If not, the returned value cannot be properly
	clamped.

SEE ALSO

	SwapShortFromLittleEndian, SwapShortFromBigEndian,
	SwapLongFromLittleEndian, SwapLongFromBigEndian

*/
template<class T1, class T2> MO_DLL_EXPORT_FUNC T1 moMin(T1 a, T2 b)
{
	return a < static_cast<T1>(b) ? a : static_cast<T1>(b);
}


template<class T1, class T2, class T3> MO_DLL_EXPORT_FUNC T1 moMin(T1 a, T2 b, T3 c)
{
	if(a < static_cast<T1>(b)) {
		return static_cast<T1>(c) < a ? static_cast<T1>(c) : a;
	}
	return static_cast<T1>(c) < static_cast<T1>(b) ? static_cast<T1>(c) : static_cast<T1>(b);
}


template<class T1, class T2> MO_DLL_EXPORT_FUNC T1 moMax(T1 a, T2 b)
{
	return a > static_cast<T1>(b) ? a : static_cast<T1>(b);
}


template<class T1, class T2, class T3> MO_DLL_EXPORT_FUNC T1 moMax(T1 a, T2 b, T3 c)
{
	if(a > static_cast<T1>(b)) {
		return static_cast<T1>(c) > a ? static_cast<T1>(c) : a;
	}
	return static_cast<T1>(c) > static_cast<T1>(b) ? static_cast<T1>(c) : static_cast<T1>(b);
}




// return b unless smaller than a or larger than c
template<class T1, class T2, class T3> MO_DLL_EXPORT_FUNC T2 moClamp(T1 a, T2 b, T3 c)
{
	if(b < static_cast<T2>(a)) {
		return static_cast<T2>(a);
	}
	if(b > static_cast<T2>(c)) {
		return static_cast<T2>(c);
	}
	return b;
}






};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_TEMPLATE_H

