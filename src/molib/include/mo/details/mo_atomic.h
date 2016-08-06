//
// File:	include/details/mo_atomic.h
// Object:	Atomic functions for Made to Order Software libraries
// Copyright:	Copyright (c) 2000-2007 Made to Order Software Corp.
//		All Rights Reserved.
//
//		This software and its associated documentation contains
//		proprietary, confidential and trade secret information
//		of Made to Order Software Corp. and except as provided by
//		written agreement with Made to Order Software Corp.
//
//		a) no part may be disclosed, distributed, reproduced,
//		   transmitted, transcribed, stored in a retrieval system,
//		   adapted or translated in any form or by any means
//		   electronic, mechanical, magnetic, optical, chemical,
//		   manual or otherwise,
//
//		and
//
//		b) the recipient is not entitled to discover through reverse
//		   engineering or reverse compiling or other such techniques
//		   or processes the trade secrets contained therein or in the
//		   documentation.
//
#pragma once
// This file is included by mo_base.h


namespace molib
{


// TODO: these defined() aren't enough since the CPU could
//	 be different than i386!? To be fixed if we want to
//	 support other platforms such as Sparcs, etc.
#if defined(MO_LINUX) || defined(WIN32) || defined(MAC_I386)
// Linux, MacOS/X i386 and MS-Windows i386+ atomic AtomicAdd(ptr, value)
// For Linux, I got this from a RedHat header created for the Boost
// library (/usr/include/c++/3.2.2/i386-redhat-linux/bits/atomicity.h)

/// Our object reference counters use this type which can be used with the moAtomicAdd() functions
typedef int		mo_atomic_word_t;


#ifdef _MSC_VER
	// TODO: The code below breaks using VC++ 7.1
	// We need something here!
	__declspec(naked) static inline mo_atomic_word_t
			moAtomicAdd(volatile mo_atomic_word_t *__mem, int __val)
	{
		__asm mov eax, 8[esp];
		__asm mov edx, 4[esp];
		__asm lock xadd [edx], eax;
		__asm add eax, 8[esp];
		__asm ret;
	}
#else
	static inline mo_atomic_word_t __attribute__ ((__unused__))
			moAtomicAdd(volatile mo_atomic_word_t *__mem, int __val)
	{
		register mo_atomic_word_t __result;
		__asm__ __volatile__ ("lock; xaddl %0,%2"
				: "=r" (__result) 
				: "0" (__val), "m" (*__mem) 
				: "memory");
		return __result + __val;
	}
#endif

#endif


#if defined(MAC) && !defined(MAC_I386)
// Mac PPC atomic AtomicAdd(ptr, value)
// For Mac, I got this from a GNU header created for the Boost
// library (/usr/include/gcc/darwin/3.3/c++/ppc-darwin/bits/atomicity.h)

// any reference count needs to use this type
typedef int		mo_atomic_word_t;


#ifdef __PPC405__
#define _STWCX "sync \n\tstwcx. "
#else
#define _STWCX "stwcx. "
#endif


static inline mo_atomic_word_t __attribute__ ((__unused__))
		moAtomicAdd(volatile mo_atomic_word_t *__mem, int __val)
{
	mo_atomic_word_t __tmp, __result;
	__asm__ __volatile__ (
		"0:\t"
		"lwarx    %0,0,%2 \n\t"
		"add%I3   %1,%0,%3 \n\t"
		_STWCX "  %1,0,%2 \n\t"
		"bne-     0b \n\t"
		: "=&b"(__tmp), "=&r"(__result)
		: "r" (__mem), "Ir"(__val)
		: "cr0", "memory");
	return __result;
}

#endif


/** \fn moAtomicAdd(volatile mo_atomic_word_t *__mem, int __val)
 *
 * \brief Safely add a value to a variable in a multithread environment.
 *
 * Atomically add 'value' to '*ptr'. This means multiple threads
 * can run concurrently and the addition will still occur only
 * in a single thread and the result will always be valid.
 *
 * The main purpose for this function is to add and subtract 1 to
 * a reference counter.
 *
 * ptr must point to a value of type mo_atomic_word_t.
 *
 * \note
 * See also The Intel Processor documentation Chapter 7
 * Multiple Processor Management; 7.1 Locked Atomic Operations
 * for more information about the Intel instructions that can
 * be used atomically. We use the XADD instruction. Other
 * available instructions are BTS, BTR, BTC, CMPXCHG, CMPXCHG8B,
 * XCHG, INC, DEC, NOT, NEG, ADD, ADC, SUB, SBB, AND, OR and XOR.
 * Except for the exchange instructions, all the others you cannot
 * be sure what the new value is unless only one thread can write
 * to the location. Remember that a MOV to or from memory is also
 * guaranteed to be atomic. So you can simply use a variable in that
 * case (when one thread writes, multiple read the same memory
 * location; as long as that location is a byte, word or doubleword.)
 *                                         
 * \return This function returns the result of the addition
 *
 * \sa molib::moMutex
 */


};		// namespace molib;

// vim: ts=8
