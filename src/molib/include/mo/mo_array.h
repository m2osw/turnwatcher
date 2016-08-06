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



#ifndef MO_ARRAY_H
#define	MO_ARRAY_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_ERROR_H
#include	"mo_error.h"
#endif


namespace molib
{



class MO_DLL_EXPORT moArrayBase : public moBase
{
public:
	class moArrayError : public moError
	{
	public:
					moArrayError(int err_num, const char *format, ...);
					moArrayError(const moArrayError& err);
	};

	typedef unsigned long	position_t;

	// a value which represents an invalid position
	static const position_t		NO_POSITION  = static_cast<position_t>(-1);

	// a maximum size so that parameter can be tested once in a while
	static const unsigned long	MAXIMUM_SIZE = 1000 * 1000;
	static const unsigned long	DEFAULT_STEP = 256;

	typedef compare_t	(*array_compare_function_t)(const void *a, const void *b);

				moArrayBase(unsigned long size, array_compare_function_t acf = 0);
				moArrayBase(const moArrayBase& array, array_compare_function_t acf = 0);
	virtual			~moArrayBase();

	moArrayBase&		operator = (const moArrayBase& array);

	virtual bool		IsSorted(void) const;
	virtual bool		IsUnique(void) const;
	bool			IsEmpty(void) const;
	virtual void		Empty(void);
	bool			Exists(const void *data) const;
	unsigned long		MaxCompare(void) const;
	unsigned long		Count(void) const;
	unsigned long		Size(void) const;
	unsigned long		Step(void) const;
	const void *		Array(void) const;
	position_t		FindNext(void) const;

	virtual position_t	Find(const void *data) const = 0;
	virtual void		Append(const moArrayBase *array);
	virtual bool		Insert(const void *data) = 0;
	void			Delete(position_t position);

	void			SetArraySize(unsigned long size);
	void			SetSize(unsigned long size);
	void			SetStep(unsigned long step);
	void *			Get(position_t position) const;
	void			GetCopy(position_t position, void *data) const;
	moArrayBase&		operator += (const void *data);
	moArrayBase&		operator += (const moArrayBase& array);

protected:
	void			Init(unsigned long size, array_compare_function_t acf);
	void			InsertAt(const void *data, position_t position);

	unsigned long		f_maximum;	// maximum number of entries before a realloc
	unsigned long		f_count;	// number of entries
	unsigned long		f_size;		// size of one entry
	unsigned long		f_step;		// number of items to add when enlarging the array
	void *			f_data;		// an array of objects
	mutable position_t	f_last_found;	// last object found or closest position
	array_compare_function_t f_order_function; // the function used to order elements of this array (if not set, use the default moBase->Compare() function)
};

typedef moSmartPtr<moArrayBase>		moArrayBaseSPtr;



class MO_DLL_EXPORT moArray : public moArrayBase
{
public:
				moArray(unsigned long size, array_compare_function_t acf = 0);
				moArray(const moArrayBase& array, array_compare_function_t acf = 0);

	virtual position_t	Find(const void *data) const;
	virtual void		Append(const moArrayBase *array);
	virtual bool		Insert(const void *data);
	virtual void		Append(const moArrayBase *array, position_t position);
	virtual bool		Insert(const void *data, position_t position);
	virtual void		Set(position_t position, const void *data);
};

typedef moSmartPtr<moArray>		moArraySPtr;



class MO_DLL_EXPORT moArrayUnique : public moArray
{
public:
				moArrayUnique(unsigned long size, array_compare_function_t acf = 0);
				moArrayUnique(const moArrayBase& array, array_compare_function_t acf = 0);

	virtual bool		IsUnique(void) const;
	virtual void		Append(const moArrayBase *array);
	virtual void		Append(const moArrayBase *array, position_t position);
	virtual bool		Insert(const void *data, position_t position);
	virtual void		Set(position_t position, const void *data);
};

typedef moSmartPtr<moArrayUnique>	moArrayUniqueSPtr;



class MO_DLL_EXPORT moSortedArray : public moArrayBase
{
public:
				moSortedArray(unsigned long size, array_compare_function_t acf);
				moSortedArray(const moArrayBase& array, array_compare_function_t acf);

	void			SetCompare(array_compare_function_t order_func);
	virtual bool		IsSorted(void) const;
	virtual position_t	Find(const void *data) const;

	virtual bool		Insert(const void *data);

protected:
	mutable position_t	f_closest;	// closest found object
};

typedef moSmartPtr<moSortedArray>	moSortedArraySPtr;



class MO_DLL_EXPORT moSortedArrayUnique : public moSortedArray
{
public:
				moSortedArrayUnique(unsigned long size, array_compare_function_t acf);
				moSortedArrayUnique(const moArrayBase& array, array_compare_function_t acf);

	virtual bool		IsUnique(void) const;
	virtual bool		Insert(const void *data);
};

typedef moSmartPtr<moSortedArrayUnique>	moSortedArrayUniqueSPtr;





};			// namespace molib;

// vim: ts=8 sw=8
#endif	// #ifndef MO_ARRAY_H

