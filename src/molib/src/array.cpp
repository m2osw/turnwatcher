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




#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_array.h"
#endif

#include	"mo/mo_array.h"






namespace molib
{


/** \class moArray
 *
 * \brief Handle an array of structures.
 *
 * This object can be used to handle an array of structures without
 * any virtual tables.
 *
 * The array can be sorted and can be grown or reduced as required.
 */


/** \class moArrayBase
 *
 * \brief The base for all the other arrays.
 *
 * This class represents the base class of all the other arrays.
 */

/** \brief Create an array.
 *
 * This constructor creates an array of the specified size.
 *
 * The \p acf parameter specifies a function to call to compare
 * two items in the list.
 *
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function used to compare items in the array.
 */
moArrayBase::moArrayBase(unsigned long size, array_compare_function_t acf)
{
	Init(size, acf);
}

/** \brief Copy an array in a new array.
 *
 * This constructor is used to copy the specified \p array
 * to this new array.
 *
 * \param[in] array The source array.
 * \param[in] acf The array compare function of the new array.
 */
moArrayBase::moArrayBase(const moArrayBase& array, array_compare_function_t acf)
	: moBase(array)
{
	Init(array.Size(), acf);

	unsigned long count = array.Count();

	SetArraySize(count);

	memcpy(f_data, array.Array(), f_size * count);	/* Flawfinder: ignore */

	f_count = count;
}


/** \brief Assign the content of an array to another.
 *
 * This function copies the content of an array (\p array) to
 * this array.
 *
 * Note that the assignment operator does NOT copy the
 * array compare function. Also, the resulting copy may
 * not be sorted properly.
 *
 * \param[in] array The source array to copy in this array.
 *
 * \return A reference to this array.
 */
moArrayBase& moArrayBase::operator = (const moArrayBase& array)
{
	if(this != &array) {
		SetArraySize(array.Count());

		memcpy(f_data, array.Array(), f_size * array.Count());	/* Flawfinder: ignore */

		f_count = array.Count();
	}

	return *this;
}


/** \brief Clean up the array object.
 *
 * This function releases the memory used to hold the array
 * in memory.
 */
moArrayBase::~moArrayBase()
{
	mo_free(f_data);
}


/** \brief Initialize a standard array.
 *
 * This constructor initializes a standard array with the
 * specified size and array compare function (acf).
 *
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function called to compare two items in the array.
 */
moArray::moArray(unsigned long size, array_compare_function_t acf)
		: moArrayBase(size, acf)
{
}


/** \brief Initialize a standard array from another array.
 *
 * This constructor initializes a standard array by copying the
 * items from another array.
 *
 * The array can specify a sort function. However, that function
 * is not used when the array is copied.
 *
 * \param[in] array The source array to copy in this array.
 * \param[in] acf The function called to compare two items in the array.
 */
moArray::moArray(const moArrayBase& array, array_compare_function_t acf)
		: moArrayBase(array, acf)
{
}


/** \class moArrayUnique
 *
 * \brief An array that accepts new items only when not already present.
 *
 * This class is used to create an array with at most one instance of each
 * item that it includes. Trying to add two items that are equal does not
 * work. The second instance will be ignored.
 */

/** \brief Initialize a unique array.
 *
 * This constructor initializes an array with the
 * specified size and array compare function (acf).
 *
 * This array accepts two item only once when they are equal.
 *
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function called to compare two items in the array.
 */
moArrayUnique::moArrayUnique(unsigned long size, array_compare_function_t acf)
		: moArray(size, acf)
{
}


/** \brief Initialize a unique array from another array.
 *
 * This constructor initializes a unique array by copying the
 * items from another array.
 *
 * The array can specify a sort function. However, that function
 * is not used when the array is copied.
 *
 * \param[in] array The source array to copy in this array.
 * \param[in] acf The function called to compare two items in the array.
 */
moArrayUnique::moArrayUnique(const moArrayBase& array, array_compare_function_t acf)
		: moArray(array, acf)
{
}



/** \class moSortedArray
 *
 * \brief An array that keeps items sorted.
 *
 * This class keeps its item sorted at all time. Each time you
 * insert a new element, it gets sorted. Duplicates are permitted.
 */

/** \brief Initialize a sorted array.
 *
 * This constructor initializes an array with the
 * specified size and array compare function (acf).
 *
 * This array sorts items as they are inserted.
 *
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function called to compare two items in the array.
 */
moSortedArray::moSortedArray(unsigned long size, array_compare_function_t acf)
		: moArrayBase(size, acf)
{
	f_closest = NO_POSITION;
}


/** \brief Initialize a sorted array from another array.
 *
 * This constructor initializes a sorted array by copying the
 * items from another array.
 *
 * The array can specify a sort function. However, that constructor
 * is not used when the array is copied and thus that array will
 * not be sorted.
 *
 * \param[in] array The source array to copy in this array.
 * \param[in] acf The function called to compare two items in the array.
 */
moSortedArray::moSortedArray(const moArrayBase& array, array_compare_function_t acf)
	: moArrayBase(array, acf)
{
	f_closest = NO_POSITION;
}


/** \class moSortedArrayUnique
 *
 * \brief An array that keeps items sorted and unique.
 *
 * This class keeps its item sorted at all time and also prevent two
 * equal items to be inserted in the same array. Each time you
 * insert a new element, it gets sorted. If the element already exists
 * in the array, it is not added.
 */

/** \brief Initialize a sorted unique array.
 *
 * This constructor initializes an array with the
 * specified size and array compare function (acf).
 *
 * This array sorts items as they are inserted and disallow
 * two equal items to be inserted.
 *
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function called to compare two items in the array.
 */
moSortedArrayUnique::moSortedArrayUnique(unsigned long size, array_compare_function_t acf)
	: moSortedArray(size, acf)
{
}


/** \brief Initialize a sorted and unique array from another array.
 *
 * This constructor initializes a sorted array by copying the
 * items from another array.
 *
 * The array can specify a sort function. However, that constructor
 * is not used when the array is copied and thus that array will
 * not be sorted (unless the source was properly sorted already.)
 *
 * \param[in] array The source array to copy in this array.
 * \param[in] acf The function called to compare two items in the array.
 */
moSortedArrayUnique::moSortedArrayUnique(const moArrayBase& array, array_compare_function_t acf)
	: moSortedArray(array, acf)
{
}





/** \brief Initialize an array whenever a constructor is called.
 *
 * This function make an array variable members coherent to the
 * system.
 * 
 * \param[in] size The size of the array at the start.
 * \param[in] acf The function used to compare items in the array.
 */
void moArrayBase::Init(unsigned long size, array_compare_function_t acf)
{
	f_maximum = 0;
	f_count = 0;
	f_size = size;
	f_step = DEFAULT_STEP;
	f_data = 0;
	f_last_found = NO_POSITION;
	f_order_function = acf;
}




/** \brief Change the size of the array.
 *
 * This function changes the size of the array used to hold the
 * objects inserted in the array. This function does not
 * change what Count() returns unless the specified size is
 * smaller than Count().
 *
 * To change the number of items (i.e. Count()'s value) call
 * the SetSize() function instead.
 *
 * It is possible to set the array size to zero in which case
 * the array will be freed.
 *
 * \note
 * The size of the array is always rounded up to the next
 * size defined by f_step. You can change the step with
 * the SetStep() function.
 *
 * \exception std::bad_alloc
 * If a memory error occurs.
 *
 * \param[in] size The number of entries we want to have in this array.
 *
 * \sa moArrayBase::Count()
 * \sa moArrayBase::SetSize()
 * \sa moArrayBase::SetStep()
 */
void moArrayBase::SetArraySize(unsigned long size)
{
	// make the size a multiple of f_step (256 by default)
	size += f_step - 1;
	size -= size % f_step;

	// NOTE: the array buffer is not allocated with the 'new' operator
	// because we want to use the realloc() capability which can be
	// much faster than new + delete in many cases.
	if(size < f_maximum) {
		if(size == 0) {
			mo_free(f_data);
			f_data = 0;
		}
		else {
			f_data = mo_realloc(f_data, size * f_size, "moArray::SetArraySize(): reducing array buffer");
		}
	}
	else if(size > 0) {
		f_data = mo_realloc(f_data, size * f_size, "moArray::SetArraySize(): enlarging array buffer");
	}

	f_maximum = size;
	if(f_count > f_maximum) {
		f_count = f_maximum;
	}
}





/** \brief Search for the next equal item.
 *
 * This function searches for the next item within a array that is
 * equal to the last item that was found. The search mechanism is
 * the same as for the Find() function.
 * 
 * In a sorted item, you can also use the Get() function to get
 * the following items and check whether they are equal to what
 * you are looking for once you reach the end of the list or
 * an item that is not equal you are done.
 *
 * \exception moArrayError(MO_ERROR_NO_FUNCTION)
 * Throw an moArrayError() whenever this function is called and the array has
 * no comparison function. It is not otherwise an error to not have a
 * comparison function defined.
 *
 * \bug
 * The comparisons may fail if an object has no comparison or
 * invalid objects are compared between each others. Usually,
 * the system throws some exception in such circumstances.
 *
 * \return This function returns the position of the found object. When
 *	   there are no item equal to the object parameter, then a value
 *	   of NO_POSITION is returned.
 *
 * \sa Exists
 */
moArrayBase::position_t moArrayBase::FindNext() const
{
	position_t	p;
	const void	*data;

// can this array be ordered?
	if(f_order_function == 0) {
		throw moArrayError(MO_ERROR_NO_FUNCTION, "no comparison function for this array");
		/*NOTREACHED*/
	}

// anything to find?
	if(f_last_found == NO_POSITION) {
		return NO_POSITION;
	}

// loop through the following (equal) objects
	data = static_cast<const char *>(f_data) + f_last_found * f_size;
	p = f_last_found + 1;
	while(p < f_count) {
		if((*f_order_function)(static_cast<const char *>(f_data) + p * f_size, data) == MO_BASE_COMPARE_EQUAL) {
			// we found one which equals the user object
#ifdef MO_CONFIG_NO_MUTABLE
			return const_cast<position_t&>(f_last_found) = p;
#else
			return f_last_found = p;
#endif
		}
		if(IsSorted()) {
			// sorted arrays can't have another object equal
			// after one which wasn't equal
			break;
		}
		p++;
	}

#ifdef MO_CONFIG_NO_MUTABLE
	return const_cast<position_t&>(f_last_found) = NO_POSITION;
#else
	return f_last_found = NO_POSITION;
#endif
}


/** \brief Search for an item in a array.
 *
 * This function searches an item within a array. In sorted arrays,
 * a binary search will be used. In unsorted arrays, each items are
 * compared to the object parameter until one is equal.
 * 
 * If more than one item in the array is equal to the object parameter
 * only one of them is returned. In unsorted arrays, the first item
 * found is returned. In sorted arrays, it is not possible to tell
 * which one is returned.
 * 
 * In unordered arrays, it is possible to find the following items
 * using the FindNext() function. In sorted arrays, one can use
 * the Get() operator and compare the following object or use the
 * FindNext() function call.
 *
 * \exception moArrayError(MO_ERROR_NO_FUNCTION)
 * Throw an moArrayError() whenever this function is called and the array has
 * no comparison function. It is not otherwise an error to not have a
 * comparison function defined.
 *
 * \exception moArrayError(MO_ERROR_COMPARE)
 * Throw an moArrayError() whenever the compare function generated an error
 * before knowing which element was smaller/larger or whether the elements
 * were equal.
 *
 * \exception moArrayError(MO_ERROR_UNORDERED)
 * Throw an moArrayError() whenever the compare function just cannot determine
 * which of two elements is the smallest/largest or whether they are equal.
 *
 * \exception moArrayError(MO_ERROR_BAD_COMPARE)
 * Throw an moArrayError() whenever the compare function returns an invalid
 * result.
 *
 * \bug
 * The comparisons may fail if an object has no comparison or
 * invalid objects are compared between each others. Usually,
 * the system throws some exception in such circumstances.
 *
 * \param[in] data Search the array for an object equal to the one pointed by data.
 *
 * \return This function returns the position of the found object. When
 *	   there are no items equal to the data parameter, then a value
 *	   of NO_POSITION is returned.
 *
 * \sa Exists
 */
moArrayBase::position_t moArray::Find(const void *data) const
{
	position_t	p;

// can this array be ordered?
	if(f_order_function == 0) {
		throw moArrayError(MO_ERROR_NO_FUNCTION, "no comparison function for this array");
		/*NOTREACHED*/
	}

// loop through all the objects
	p = 0;
	while(p < f_count) {
		if((*f_order_function)(static_cast<const char *>(f_data) + p * f_size, data) == MO_BASE_COMPARE_EQUAL) {
			// we found one which equals the user object
#ifdef MO_CONFIG_NO_MUTABLE
			return const_cast<position_t&>(f_last_found) = p;
#else
			return f_last_found = p;
#endif
		}
		p++;
	}

#ifdef MO_CONFIG_NO_MUTABLE
	return const_cast<position_t&>(f_last_found) = NO_POSITION;
#else
	return f_last_found = NO_POSITION;
#endif
}


/** \brief Search an item in an array.
 *
 * \copydoc moArray::Find(const void *data) const
 */
moArray::position_t moSortedArray::Find(const void *data) const
{
	position_t	i, j, p;
	compare_t	r;

// can this array be ordered?
	if(f_order_function == 0) {
		throw moArrayError(MO_ERROR_NO_FUNCTION, "no comparison function for this array");
		/*NOTREACHED*/
	}

// we've got to define p here to avoid warnings
	p = 0;

// too few items for the binary search to be effective? (and work properly!)
	if(f_count < 4) {
		// loop through all the objects
		while(p < f_count) {
			r = (*f_order_function)(static_cast<const char *>(f_data) + p * f_size, data);
			if(r == MO_BASE_COMPARE_EQUAL) {
				i = p + 1;
				if(!IsUnique()) {
					while(i < f_count
					&& (*f_order_function)(static_cast<const char *>(f_data) + i * f_size, data) == MO_BASE_COMPARE_EQUAL) {
						i++;
					}
				}
#ifdef MO_CONFIG_NO_MUTABLE
				const_cast<position_t&>(f_closest) = i;
#else
				f_closest = i;	// insert after the item we found
#endif
				// return the first item equal to the
				// given object
				while(p > 0
				&& (*f_order_function)(static_cast<const char *>(f_data) + (p - 1) * f_size, data) == MO_BASE_COMPARE_EQUAL) {
					p--;
				}
#ifdef MO_CONFIG_NO_MUTABLE
				return const_cast<position_t&>(f_last_found) = p;
#else
				return f_last_found = p;
#endif
			}
			if(r == MO_BASE_COMPARE_GREATER) {
				break;
			}
			if(r != MO_BASE_COMPARE_SMALLER) {
				switch(r) {
				case MO_BASE_COMPARE_ERROR:
					throw moArrayError(MO_ERROR_COMPARE, "moSortedArray::Find(): comparison generated an error");

				case MO_BASE_COMPARE_UNORDERED:
					throw moArrayError(MO_ERROR_UNORDERED, "moSortedArray::Find(): comparison cannot determine proper order");

				default:
					throw moArrayError(MO_ERROR_BAD_COMPARE, "moSortedArray::Find(): unknown comparison result");

				}
				/*NOTREACHED*/
			}
			p++;
		}
#ifdef MO_CONFIG_NO_MUTABLE
		const_cast<position_t&>(f_closest) = p;
		return const_cast<position_t&>(f_last_found) = NO_POSITION;
#else
		f_closest = p;	// insert here (possibly at the end)
		return f_last_found = NO_POSITION;
#endif
	}

// a sorted array is searched with a quick binary search algorithm
	i = 0;
	j = f_count;
	while(i < j) {
		// get the center position of the current range
		p = i + (j - i) / 2;
		r = (*f_order_function)(static_cast<const char *>(f_data) + p * f_size, data);
		switch(r) {
		case MO_BASE_COMPARE_EQUAL:
			// make sure new items are inserted last
			i = p + 1;
			if(!IsUnique()) {
				while(i < f_count
				&& (*f_order_function)(static_cast<const char *>(f_data) + i * f_size, data) == MO_BASE_COMPARE_EQUAL) {
					i++;
				}
			}
#ifdef MO_CONFIG_NO_MUTABLE
			const_cast<position_t&>(f_closest) = i;
#else
			f_closest = i;	// insert after the item we found
#endif
			// return the first item equal to the
			// given object
			while(p > 0
			&& (*f_order_function)(static_cast<const char *>(f_data) + (p - 1) * f_size, data) == MO_BASE_COMPARE_EQUAL) {
				p--;
			}
#ifdef MO_CONFIG_NO_MUTABLE
			return const_cast<position_t&>(f_last_found) = p;
#else
			return f_last_found = p;
#endif

		case MO_BASE_COMPARE_SMALLER:
			// move the range up (we already checked p so use p + 1)
			p++;
			i = p;
			break;

		case MO_BASE_COMPARE_GREATER:
			// move the range down (we never check an item at position j)
			j = p;
			break;

		case MO_BASE_COMPARE_ERROR:
			throw moArrayError(MO_ERROR_COMPARE, "moSortedArray::Find(): comparison generated an error");

		case MO_BASE_COMPARE_UNORDERED:
			throw moArrayError(MO_ERROR_UNORDERED, "moSortedArray::Find(): comparison cannot determine proper order");

		default:
			throw moArrayError(MO_ERROR_BAD_COMPARE, "moSortedArray::Find(): unknown comparison result");

		}
	}

	// insert before this item we found is larger than 'object'
#ifdef MO_CONFIG_NO_MUTABLE
	const_cast<position_t&>(f_closest) = p;
	return const_cast<position_t&>(f_last_found) = NO_POSITION;
#else
	f_closest = p;
	return f_last_found = NO_POSITION;
#endif
}




/** \brief Low level insert function.
 *
 * This function is an internal function used to insert an object
 * in the array buffer. It should only be used by the internal
 * Insert() functions that make sure the position is correct
 * and insertion is possible (i.e. in unique arrays, items can be
 * inserted only once).
 * 
 * \exception std::bad_alloc()
 * If the buffer cannot be enlarged.
 * 
 * \param[in] data The object to insert.
 * \param[in] position Position at which the item is inserted.
 * 
 * \sa moArrayBase::Insert()
 */
void moArrayBase::InsertAt(const void *data, position_t position)
{
// append item?
	if(position > f_count) {
		position = f_count;
	}

// enough room?
	if(f_count + 1 > f_maximum) {
		SetArraySize(f_count + 1);
	}

// need to move data?
	if(position < f_count) {
		memmove(static_cast<char *>(f_data) + (position + 1) * f_size,
				static_cast<const char *>(f_data) + position * f_size,
					f_size * (f_count - position));
	}

// save the object pointer in the array
	memcpy(static_cast<char *>(f_data) + position * f_size, data, f_size);	/* Flawfinder: ignore */
	f_count++;
}



/** \brief Insert an item in the array.
 *
 * The Insert() functions insert an item at the specified
 * position.
 *
 * Note that the functions return false if the item is
 * not inserted. This happens in Unique arrays that refuse
 * to insert two items that are equal.
 *
 * Sorted array will insert the new item at a calculated
 * position that keeps the array sorted. When a position
 * is specified as a parameter, it is ignored.
 *
 * \param[in] data The data to insert in this array
 *
 * \return true when the item was inserted.
 */
bool moArray::Insert(const void *data)
{
	// we've got to call Insert() here so moArrayUnique::Insert()
	// is called as required
	return Insert(data, NO_POSITION);
}


/** \brief Insert an item at the specified position.
 *
 * \copydoc moArray::Insert(const void *data)
 *
 * \param[in] data The data to insert in this array.
 * \param[in] position The position at which the item shall be inserted.
 */
bool moArray::Insert(const void *data, position_t position)
{
	// insert the object at that position
	moArray::InsertAt(data, position);
	return true;
}


/** \brief Insert an item at the specified position.
 *
 * \copydoc moArray::Insert(const void *data, position_t position)
 */
bool moArrayUnique::Insert(const void *data, position_t position)
{
	if(!Exists(data)) {		// insert only if not found
		moArray::InsertAt(data, position);
		return true;
	}

	return false;
}


/** \brief Insert an item at the specified position.
 *
 * \copydoc moArray::Insert(const void *data)
 */
bool moSortedArray::Insert(const void *data)
{
 	Find(data);			// define f_closest
	InsertAt(data, f_closest);	// insert the object now
	f_closest = NO_POSITION;
	return true;
}


/** \brief Insert an item at the specified position.
 *
 * \copydoc moArray::Insert(const void *data)
 */
bool moSortedArrayUnique::Insert(const void *data)
{
	if(!Exists(data)) {		// define f_closest
		InsertAt(data, f_closest);	// insert only if not found
		f_closest = NO_POSITION;
		return true;
	}
	f_closest = NO_POSITION;
	return false;
}




/** \brief Append an array to this array.
 *
 * This function appends the specified \p array to this
 * array. The array is appended at the end unless this
 * array is sorted. In that case, the item are inserted
 * so the array remains sorted.
 *
 * \param[in] array The array to insert.
 */
void moArray::Append(const moArrayBase *array)
{
	Append(array, NO_POSITION);
}



/** \brief Append an array to this array.
 *
 * \copydoc moArray::Append(const moArrayBase *array)
 */
void moArrayUnique::Append(const moArrayBase *array)
{
	moArrayBase::Append(array);
}


/** \brief Append an array to this array.
 *
 * \copydoc moArray::Append(const moArrayBase *array)
 *
 * \param[in] position Where the append starts inserting items in this array.
 */
void moArray::Append(const moArrayBase *array, position_t position)
{
// anything to append?
	if(array == 0 || array->Count() == 0) {
		return;
	}

// append item?
	if(position > f_count) {
		position = f_count;
	}

// enough room?
	if(f_count + array->Count() > f_maximum) {
		SetArraySize(f_count + array->Count());
	}

// need to move data?
	if(position < f_count) {
		memmove(static_cast<char *>(f_data) + (position + array->Count()) * f_size,
				static_cast<const char *>(f_data) + position * f_size,
				f_size * (f_count - position));
	}

// copy the other array object pointers
	memcpy(static_cast<char *>(f_data) + position * f_size,		/* Flawfinder: ignore */
			array->Array(), f_size * array->Count());

	f_count += array->Count();
}


/** \brief Append an array to this array.
 *
 * \copydoc moArray::Append(const moArrayBase *array)
 */
void moArrayBase::Append(const moArrayBase *array)
{
	position_t	i, max;

// anything to insert?
	if(array == 0) {
		return;
	}

// insert items one by one using the Insert function
	max = array->Count();
	i = 0;
	while(i < max) {
		Insert(array->Get(i));
		i++;
	}
}



/** \brief Append an array to this array.
 *
 * \copydoc moArray::Append(const moArrayBase *array)
 *
 * \param[in] position Where the append starts inserting items in this array.
 */
void moArrayUnique::Append(const moArrayBase *array, position_t position)
{
	position_t	i, max;
	unsigned long	offset;

// anything to insert?
	if(array == 0) {
		return;
	}

// insert items one by one using the Insert function
	if(position > f_count) {
		position = f_count;
	}
	offset = f_count - position;	// used to always insert at the same place
	max = array->Count();
	i = 0;
	while(i < max) {
		Insert(array->Get(i), f_count - offset);
		i++;
	}
}


/** \brief delete an item in a array
 *
 * This function deletes the item at the specified position.
 * 
 * An invalid position will be silently ignored. This is
 * because in the insertion functions, a position out of
 * bounds is used to append items.
 *
 * \note
 * The buffer holding the array of objects is never reduced
 * by this function.
 *
 * \bug
 * The item is removed from the array. However, if the item
 * has some resources that need to be released, that has to
 * be done before calling this function.
 *
 * \param[in] position The position of the item to be deleted.
 *
 * \sa Empty()
 * \sa SetSize()
 */
void moArrayBase::Delete(position_t position)
{
	if(position >= f_count) {
		return;
	}

	f_count--;
	if(position < f_count) {
		memmove(static_cast<char *>(f_data) + position * f_size,
			static_cast<const char *>(f_data) + (position + 1) * f_size,
			(f_count - position) * f_size);
	}
}



/** \brief Set how fast the array should be enlarged.
 *
 * This function sets the step parameter of the array. At the start
 * all arrays have a step of DEFAULT_STEP (256 at this time).
 * 
 * The step is used whenever the array needs to be enlarged to
 * accept one or more items. The number of items added will include
 * the requested size rounded up to the nearest multiple of this
 * step value.
 * 
 * The step can't be set to a value smaller than 256 and should
 * not be set to too large a value either.
 * 
 * \bug
 * The size of the array isn't changed when this function is
 * called. It will be only the next time it is necessary to
 * enlarge the buffer to hold more entries.
 * 
 * \param[in] step The new step value.
 * 
 * \sa moArrayBase::Step()
 * \sa moArrayBase::Size()
 * \sa moArrayBase::SetSize()
 * \sa moArrayBase::SetArraySize()
 */
void moArrayBase::SetStep(unsigned long step)
{
	// the lower limit is 256
	f_step = step < 256 ? 256 : step;
}




/** \brief Set the number of items in a array.
 *
 * This function changes the number of items present in a array.
 * The Count() function will then return this value.
 * 
 * If necessary, the array will be enlarged in order to accept the
 * request.
 * 
 * When a array is made larger, new entries are cleared (all zeroes).
 * 
 * When a array is made smaller, the last items are lost. It
 * is the user responsability to release resources held by these items
 * before a call to this function, if required.
 *
 * \exception moBug(MO_ERROR_OVERFLOW)
 * This exception is thrown whenever the \p size parameter for the
 * array is way too large (See moArrayBase::MAXIMUM_SIZE.)
 *
 * \bug
 * Sorted arrays may not like to have entries which are zero
 * pointers. It isn't wise to use this function on sorted arrays.
 *
 * \sa Insert()
 * \sa SetArraySize()
 */
void moArrayBase::SetSize(unsigned long size)
{
	if(size <= f_count) {
		f_count = size;
	}
	else if(size > MAXIMUM_SIZE) {
		throw moBug(MO_ERROR_OVERFLOW,
"moArrayBase::SetSize(): size too large (accepts a maximum of %lu entries; %lu was specified)",
						MAXIMUM_SIZE, size);
	}
	else {
		// ensure it will fit
		SetArraySize(size);
		// clear the extra entries (we know that size > f_count)
		memset(static_cast<char *>(f_data) + f_count * f_size, 0,
					(size - f_count) * f_size);
		// set the new size
		f_count = size;
	}
}




/** \brief Define the offset of a compare function.
 *
 * Changes the default Compare() function call to this
 * one. The \p order_function parameter is an offset to
 * one of the compare functions of the given object.
 * 
 * This is primarily used to sort objects with different
 * criteria in several different arrays.
 * 
 * \exception moBug(MO_ERROR_COMPARE)
 * A comparison function can only be set in an empty array.
 * It is a major error to try to change the compare function
 * of a non-empty array and for this reason this function
 * throws an MO_ERROR_COMPARE error if it ever happens.
 * Note also that this is considered a bug.
 * 
 * \param[in] order_function A member function pointer.
 * 
 * \sa IsSorted()
 */
void moSortedArray::SetCompare(array_compare_function_t order_function)
{
	if(f_count > 0) {
		throw moBug(MO_ERROR_COMPARE, "moSortedArray: error: cannot change array order function when array is not empty");
	}

	f_order_function = order_function;
}



/** \brief Return true if the array is sorted.
 *
 * This function returns true whenever the array you are dealing
 * with ensures that all the items are sorted. Objects that are
 * equal may appear in any order if the array is not also unique.
 *
 * \return true if the array is unique.
 */
bool moArrayBase::IsSorted(void) const
{
	return false;
}


/** \brief Return true if the array is sorted.
 *
 * This function returns true whenever the array you are dealing
 * with ensures that all the items are sorted. Objects that are
 * equal may appear in any order if the array is not also unique.
 *
 * \return true if the array is unique.
 */
bool moSortedArray::IsSorted(void) const
{
	return true;
}


/** \brief Return true if the array is unique.
 *
 * This function returns true whenever the array you are dealing
 * with ensures that all the items are distinct (non-equal).
 *
 * \return true if the array is unique.
 */
bool moArrayBase::IsUnique(void) const
{
	return false;
}


/** \brief Return true if the array is unique.
 *
 * This function returns true whenever the array you are dealing
 * with ensures that all the items are distinct (non-equal).
 *
 * \return true if the array is unique.
 */
bool moArrayUnique::IsUnique(void) const
{
	return true;
}


/** \brief Return true if the array is unique.
 *
 * This function returns true whenever the array you are dealing
 * with ensures that all the items are distinct (non-equal).
 *
 * \return true if the array is unique.
 */
bool moSortedArrayUnique::IsUnique(void) const
{
	return true;
}


/** \brief True if the array is empty.
 *
 * This function returns true whenever the array is empty.
 *
 * \return true if the array has 0 items.
 */
bool moArrayBase::IsEmpty(void) const
{
	return f_count == 0;
}


/** \brief Make the array empty.
 *
 * This function clears the content of the array.
 *
 * You are responsible to release any resources used by the items
 * before clearing them.
 */
void moArrayBase::Empty(void)
{
	f_count = 0;
}


/** \brief The number of items currently defined in the array.
 *
 * This function returns the exact number of items currently defined
 * in the array.
 *
 * Use SetSize() to change that number.
 *
 * \return The number of items in the array.
 */
unsigned long moArrayBase::Count(void) const
{
	return f_count;
}


/** \brief The size of one item in the array.
 *
 * This function returns the size of an item present in the array.
 *
 * \return The size in bytes of an item in the array.
 */
unsigned long moArrayBase::Size(void) const
{
	return f_size;
}


/** \brief The minimum number of items to allocate.
 *
 * This function returns the minimum number of items that the
 * SetArraySize() will allocate.
 *
 * Also, each time the buffer is enlarged, it will be made
 * equal to a multiple of this value.
 *
 * \return The step used to allocate the new array buffers.
 */
unsigned long moArrayBase::Step(void) const
{
	return f_step;
}


/** \brief Return the maximum number of compares to be done to find an item in an array.
 *
 * This function returns the number of items that the system will
 * compare before to return from a Find() function call.
 * 
 * This assumes the item being searched is unique. If not, then
 * additional comparisons will occur to find the first instance of
 * equal items.
 * 
 * Arrays that aren't sorted require the Find() to search all the
 * items sequentially. In this case, this function returns the
 * same value as Count().
 *
 * \note
 * It is valid to call this function with a null pointer as this
 * array.
 *
 * \return The maximum number of compares necessary in a Find().
 *
 * \sa Count()
 * \sa IsSorted()
 */
unsigned long moArrayBase::MaxCompare(void) const
{
	if(this == 0) {
		return 0;
	}
	if(IsSorted() && f_count >= 4UL) {
#ifdef _MSC_VER
		return static_cast<unsigned long>(ceil(log(static_cast<long double>(f_count)) / log(2.0)));
#else
		return static_cast<unsigned long>(ceil(log(static_cast<unsigned long>(f_count)) / log(2)));
#endif
	}
	return static_cast<unsigned long>(f_count);
}


/** \brief Teturns the array pointer.
 *
 * This function returns a pointer to the internal array of
 * structures.
 * 
 * It is unwise to try to modify the buffer. The number of
 * items in the buffer is as returned by Count(). This buffer
 * should only be used as a read-only buffer. This pointer
 * will change if other functions are called.
 *
 * \bug
 * This function should not exists, yet it simplifies the
 * Append() function so it is possible to copy an array in
 * another very quickly.
 * 
 * The pointer will be invalidated if other functions are
 * called on this array (any function which may call the
 * SetArraySize() to be precise).
 *
 * \return a pointer to the array of object;
 *	the pointer can be zero (0) if the array is empty
 *
 * \sa moArrayBase::Count() const
 */
const void *moArrayBase::Array(void) const
{
	return f_data;
}


/** \brief Checks whether a given object is present in a array.
 *
 * The Exists() function searches for the data parameter and returns
 * true if it finds it; false otherwise.
 *
 * This function is used by the different functions trying to insert
 * a new item in an array if the array is unique. This is used to
 * ensure that two equal items are not inserted twice in such array.
 *
 * \param[in] data A structure of the data to search.
 *
 * \return true when the given object is present in the array
 */
bool moArrayBase::Exists(const void *data) const
{
	return Find(data) != NO_POSITION;
}


/** \brief Append an item to this array.
 *
 * This operator inserts the specified item to the array.
 * 
 * In the second instance, all of the items present in the array
 * specified as a parameter are appended to this array. Note that
 * the items are appended in the order they are found in the
 * array parameter. That is significant only if the destination
 * array is not sorted. It may be faster to call this function when
 * the destination array is not sorted than to add items one by
 * one.
 * 
 * This function uses the Insert() function.
 *
 * \bug
 * To insert an array as an item you need to cast the array into
 * an moBase object to avoid calling the second instance of
 * the += operator:
 * 
 * \code
 *	array1 += dynamic_cast<const moBase&>(array2);
 * \endcode
 * 
 * \par
 * It may also be wise to use the Insert() function in this
 * case:
 *
 * \code
 * 	array1->Insert(array2);
 * \endcode
 *
 * \sa Insert()
 * \sa Append()
 */
moArrayBase& moArrayBase::operator += (const void *data)
{
	Insert(data);

	return *this;
}


/** \brief Append an array to this array.
 *
 * This operator inserts all of the items present in the \p array
 * specified as a parameter to this array. Note that
 * the items are appended in the order they are found in the
 * source \p array. That is significant only if the destination
 * array is not sorted. It may be faster to call this function when
 * the destination array is not sorted than to add items one by
 * one.
 * 
 * This function uses one of the Append() functions.
 *
 * \bug
 * To insert an array as an item you need to cast the array into
 * an moBase object to avoid calling the second instance of
 * the += operator:
 * 
 * \code
 *	array1 += dynamic_cast<const moBase&>(array2);
 * \endcode
 * 
 * \par
 * It may also be wise to use the Insert() function in this
 * case:
 *
 * \code
 * 	array1->Insert(array2);
 * \endcode
 *
 * \sa Insert()
 * \sa Append()
 */
moArrayBase& moArrayBase::operator += (const moArrayBase& array)
{
	Append(&array);

	return *this;
}


/** \brief get a array item
 *
 * The Get() function returns a pointer to one of the array
 * objects. The \p position parameter has to be between zero and
 * Count() - 1.
 *
 * \bug
 * This function returns a pointer to the content of the array buffer.
 * If you add a new item in the array, it is not unlikely that the
 * pointers you get with Get() will become invalid.
 *
 * \exception moBug(MO_ERROR_OVERFLOW)
 * Throw an moBug(MO_ERROR_OVERFLOW) whenever the position is out of
 * bounds.
 *
 * \param[in] position Zero based position of the object to retrieve.
 *
 * \return A pointer to the object in the array (i.e. not a copy!)
 */
void *moArrayBase::Get(position_t position) const
{
	if(position >= f_count) {
		throw moBug(MO_ERROR_OVERFLOW, "moArrayBase: position out of bounds");
	}

	return static_cast<char *>(f_data) + position * f_size;
}


/** \brief Get the copy of an item.
 *
 * This function is used to copy the item at the specified \p position
 * into the \p data buffer.
 *
 * \exception moBug(MO_ERROR_OVERFLOW)
 * If the position is too large. Note that this could be viewed as a
 * runtime error. Yet it is marked as a bug since it should never occur
 * and thus you should not try to catch this exception in an attempt
 * of making your code function better.
 *
 * \param[in] position The position of the item to copy.
 * \param[in] data The pointer to a buffer to fill with the item data.
 */
void moArrayBase::GetCopy(position_t position, void *data) const
{
	if(position >= f_count) {
		throw moBug(MO_ERROR_OVERFLOW, "moArrayBase: position out of bounds");
	}

	memcpy(data, static_cast<char *>(f_data) + position * f_size, f_size);	/* Flawfinder: ignore */
}


/** \brief Set an item to the specified data.
 *
 * This function is used to copy the \p data in an item at the
 * specified \p position. If the array is smaller, then it is
 * enlarged to accomodate the position.
 *
 * \param[in] position The position of the item to copy.
 * \param[in] data The pointer to a buffer to copy in the specified item.
 *
 * \sa moArray::SetSize()
 */
void moArray::Set(position_t position, const void *data)
{
	if(position >= f_count) {
		SetSize(position + 1);
	}

	memcpy(static_cast<char *>(f_data) + position * f_size, data, f_size);	/* Flawfinder: ignore */
}



/** \brief Set an item to the specified data.
 *
 * This function is used to copy the \p data in an item at the
 * specified \p position. If the array is smaller, then it is
 * enlarged to accomodate the position.
 *
 * For this type of array, the data is copied only if it does
 * not already exist in the array.
 *
 * \param[in] position The position of the item to copy.
 * \param[in] data The pointer to a buffer to copy in the specified item.
 *
 * \sa moArray::SetSize()
 */
void moArrayUnique::Set(position_t position, const void *data)
{
	if(!Exists(data)) {
		moArray::Set(position, data);
	}
}





/** \class moArrayBase::moArrayError
 *
 * \brief Errors thrown by the moArray class
 *
 * To specifically catch exceptions from moArray objects, you can catch
 * the moArrayError instead of moError.
 */


/** \brief Initializes an moArrayError exception.
 *
 * This constructor defines an moArrayError with the specified
 * error number and format string.
 *
 * \param[in] err_num The error number for this error.
 * \param[in] format The message string format followed by the necessary arguments.
 */
moArrayBase::moArrayError::moArrayError(int err_num, const char *format, ...)
	: moError(err_num)
{
	va_list		params;

	va_start(params, format);
	Init(format, params);
	va_end(params);
}


/** \brief Initializes an moArrayError exception.
 *
 * This constructor creates a clone of the input error.
 *
 * \param[in] err The error to copy.
 */
moArrayBase::moArrayError::moArrayError(const moArrayError& err)
	: moError(err)
{
}





};			// namespace molib;

// vim: ts=8 sw=8 syntax=cpp.doxygen

