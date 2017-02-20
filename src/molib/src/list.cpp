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
#pragma implementation "mo/mo_list.h"
#endif

#include	"mo/mo_list.h"


namespace molib
{



// auto-documented by moBase same name function.
const char *moListBase::moGetClassName(void) const
{
	return "molib::moBase::moListBase";
}

const char *moList::moGetClassName(void) const
{
	return "molib::moBase::moListBase::moList";
}

const char *moSortedList::moGetClassName(void) const
{
	return "molib::moBase::moListBase::moSortedList";
}

const char *moListUnique::moGetClassName(void) const
{
	return "molib::moBase::moListBase::moList::moListUnique";
}

const char *moSortedListUnique::moGetClassName(void) const
{
	return "molib::moBase::moListBase::moSortedList::moSortedListUnique";
}


/** \brief Ensure that the list buffer is large enough.
 *
 * This function changes the size of the array used to hold the
 * object pointers inserted in the list. This function does not
 * change what Count() returns unless the specified size is
 * smaller than the current Count().
 * 
 * To change the number of items (i.e. Count()'s value) call
 * the SetSize() function instead.
 * 
 * It is possible to set the array size to zero in which case
 * the array will be freed.
 * 
 * The exact size of the array is determined by how the internal
 * library manages the minimum number of entries and the number
 * of entries allocated at once (i.e. multiple of 256).
 * 
 * The function will try to enlarge or reduce the size of the
 * buffer as required.
 * 
 * \note
 * When a memory error occurs, this function will throw an std::bad_alloc
 * exception.
 *
 * \param[in] size The number of entries we want to have in this list.
 *
 * \sa SetSize()
 * \sa Count() const
 *
 */
void moListBase::SetArraySize(unsigned long size)
{
	moBase		**p;

	// make the size a multiple of 256 (we may change that into a
	// variable member later)
	size = (size + 255) & -256;

	// NOTE: the array buffer is not allocated with the 'new' operator
	// because we want to use the realloc() capability which can be
	// much faster than new + delete in many cases.
	if(size < f_maximum) {
		// we are about to reduce the size of the buffer
		// and thus we need to release objects present
		// at the end of the list
		while(f_count > size) {
			f_count--;
			f_data[f_count]->Release();
		}
		if(size == 0) {
			mo_free(f_data);
			f_data = 0;
		}
		else {
			p = static_cast<moBase **>(mo_realloc(f_data, size * sizeof(moBase *), "moList::SetArraySize: list array (1)"));
			f_data = p;
			f_maximum = size;
		}
	}
	else if(size > 0) {
		if(f_data == 0) {
			p = static_cast<moBase **>(mo_malloc(size * sizeof(moBase *), "moList::SetArraySize: list array (2)"));
		}
		else {
			p = static_cast<moBase **>(mo_realloc(f_data, size * sizeof(moBase *), "moList::SetArraySize: list array (3)"));
		}
		f_data = p;
		f_maximum = size;
		// here f_maximum is enlarged and can't be smaller than f_count
	}
}


/** \brief Compare two lists objects.
 *
 * This function expects a list as its object parameter. It will
 * then compare both lists against each others. To do so, it
 * compares corresponding items and depending on the results
 * return whether this list is smaller, equal or larger to the
 * \p list parameter, which is the same as the last unequal
 * comparison of two items or it returns that they are equal.
 *
 * \note
 * The Compare() function is a list can be changed. This function
 * is the default and calls ->Compare() not ->*Compare(). This
 * means a user implemented function (->*Compare()) can call this
 * Compare() function if required.
 * 
 * \param[in] object The other list to compare with this list.
 *
 * \return One of the comparison values:
 *		MO_BASE_COMPARE_ERROR,
 *		MO_BASE_COMPARE_SMALLER,
 *		MO_BASE_COMPARE_EQUAL,
 *		MO_BASE_COMPARE_GREATER or
 *		MO_BASE_COMPARE_UNORDERED.
 */
moBase::compare_t moListBase::Compare(const moBase& object) const
{
	const moList&	list = dynamic_cast<const moList&>(object);
	compare_t	r;
	position_t	i;

	// very fast equality test - note: improperly avoids problems of
	// unordered objects
	if(this == &list) {
		return MO_BASE_COMPARE_EQUAL;
	}

	i = 0;
	while(i < f_count && i < list.f_count) {
		r = f_data[i]->Compare(*list.f_data[i]);
		if(r != MO_BASE_COMPARE_EQUAL) {
			return r;
		}
	}

	if(f_count == list.f_count) {
		return MO_BASE_COMPARE_EQUAL;
	}

	return f_count < list.f_count ? MO_BASE_COMPARE_SMALLER
				      : MO_BASE_COMPARE_GREATER;
}



/** \brief Search for the next equal item.
 *
 * This function searches a non-sorted list for an item that is equal
 * to the last item found with the Find() function.
 *
 * This function is slow and you should consider using a sorted list
 * if you need to do extensive searches in your list.
 *
 * \bug
 * The sort algorithm is expected not to change during the whole life
 * of the list.
 *
 * \return The position of the next object that is equal or moList::NO_POSITION.
 */
moListBase::position_t moListBase::FindNext(void) const
{
	position_t	p;
	const moBase	*object;

// anything to find?
	if(f_last_found == NO_POSITION) {
		return NO_POSITION;
	}

// loop through the following (equal) objects
	object = f_data[f_last_found];
	p = f_last_found + 1;
	while(p < f_count) {
		if(*f_data[p] == *object) {
			// we found one which equals the user object
			return f_last_found = p;
		}
		p++;
	}

	return f_last_found = NO_POSITION;
}


/** \brief Search for an object in a non-sorted list.
 *
 * This function search for an object in a non-sorted list.
 *
 * This function compares the specified \p object to each items
 * starting at item 0 until it finds one that is equal.
 *
 * \note
 * If you have to use a list in which you need to extensively search
 * for items, you may want to consider using a sorted list instead.
 *
 * \bug
 * The sort algorithm is expected not to change during the whole life
 * of the list.
 *
 * \param[in] object The object to search for.
 *
 * \return The position at which the object is found or moList::NO_POSITION.
 */
moListBase::position_t moList::Find(const moBase *object) const
{
	position_t	p;

// loop through all the objects
	p = 0;
	while(p < f_count) {
		if(*f_data[p] == *object) {
			// we found one which equals the user object
			return f_last_found = p;
		}
		p++;
	}

	return f_last_found = NO_POSITION;
}


/** \brief Find the next object that is equal.
 *
 * This function checks to see whether the next object is equal to the
 * last found object. If so, it returns its position that also becomes
 * the position of the last found object.
 *
 * This function works only after a call to Find().
 *
 * \bug
 * The sort algorithm is expected not to change during the whole life
 * of the list.
 *
 * \return The next object that is equal or moList::NO_POSITION.
 */
moListBase::position_t moSortedList::FindNext(void) const
{
	position_t	p;
	const moBase	*object;
	compare_t	r;

// anything to find?
	if(f_last_found == NO_POSITION) {
		return NO_POSITION;
	}

// check the next object only (in a sorted list)
	object = f_data[f_last_found];
	p = f_last_found + 1;
	if(p < f_count) {
		if(f_order_function != 0) {
			r = (f_data[p]->*f_order_function)(*object);
		}
		else {
			r = f_data[p]->Compare(*object);
		}
		if(r == MO_BASE_COMPARE_EQUAL) {
			// we found one which equals the user object
			return f_last_found = p;
		}
	}

	return f_last_found = NO_POSITION;
}


/** \brief Search for an item in this list.
 *
 * This function uses a binary search to find the specified \p object
 * in this sorted list of items.
 *
 * \bug
 * The sort algorithm is expected not to change during the whole life
 * of the list.
 *
 * \return The position of an equal object or moList::NO_POSITION
 */
moList::position_t moSortedList::Find(const moBase *object) const
{
	static const char bad_compare_result[] = "moSortedList::Find(): comparison function returned an unknown comparison result";
	position_t	i, j, p;
	compare_t	r;

// we've got to define p here to avoid warnings
	p = 0;

// too few items for the binary search to be effective? (and work properly!)
	if(f_count < 4UL) {
		// loop through all the objects
		while(p < f_count) {
			if(f_order_function != 0) {
				r = (f_data[p]->*f_order_function)(*object);
			}
			else {
				r = f_data[p]->Compare(*object);
			}
			if(r == MO_BASE_COMPARE_EQUAL) {
				goto found;
			}
			if(r == MO_BASE_COMPARE_GREATER) {
				break;
			}
			if(r != MO_BASE_COMPARE_SMALLER) {
				switch(r) {
				case MO_BASE_COMPARE_ERROR:
					throw moError(MO_ERROR_COMPARE);

				case MO_BASE_COMPARE_UNORDERED:
					throw moError(MO_ERROR_UNORDERED);

				default:
					throw moBug(MO_ERROR_BAD_COMPARE, bad_compare_result);

				}
				/*NOTREACHED*/
			}
			p++;
		}
		f_closest = p;	// insert here (possibly at the end)
		return f_last_found = NO_POSITION;
	}

// a sorted list is searched with a quick binary search algorithm
	i = 0;
	j = f_count;
	while(i < j) {
		// get the center position of the current range
		p = i + (j - i) / 2;
		if(f_order_function != 0) {
			r = (f_data[p]->*f_order_function)(*object);
		}
		else {
			r = f_data[p]->Compare(*object);
		}
		switch(r) {
		case MO_BASE_COMPARE_EQUAL:
			// make sure new items are inserted last
found:
			i = p + 1;
			if(!IsUnique()) {
				while(i < f_count) {
					if(f_order_function != 0) {
						r = (f_data[i]->*f_order_function)(*object);
					}
					else {
						r = f_data[i]->Compare(*object);
					}
					if(r != MO_BASE_COMPARE_EQUAL) {
						break;
					}
					i++;
				}
				// return the first item equal to the
				// given object
				while(p > 0UL) {
					if(f_order_function != 0) {
						r = (f_data[p - 1]->*f_order_function)(*object);
					}
					else {
						r = f_data[p - 1]->Compare(*object);
					}
					if(r != MO_BASE_COMPARE_EQUAL) {
						break;
					}
					p--;
				}
			}
			f_closest = i;	// insert after the item we found
			return f_last_found = p;

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
			throw moError(MO_ERROR_COMPARE);

		case MO_BASE_COMPARE_UNORDERED:
			throw moError(MO_ERROR_UNORDERED);

		default:
			throw moBug(MO_ERROR_BAD_COMPARE, bad_compare_result);

		}
	}

	// insert before this item we found is larger than 'object'
	f_closest = p;
	return f_last_found = NO_POSITION;
}




/** \brief Internal function to insert an item in a list.
 *
 * This function inserts the specified object at the specified position.
 * This function is used by all the Insert() functions whether the list
 * is sorted or not. This is the only function that inserts an object in
 * the array of pointers in the list.
 *
 * The insertion itself cannot fail. The function may throw an exception
 * such as a memory allocation exception.
 *
 * \param[in] object The object to insert at the specified position.
 * \param[in] position The position at which the object is to be inserted.
 */
void moListBase::InsertAt(const moBase *object, position_t position)
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
		memmove(f_data + position + 1, f_data + position,
				sizeof(moBase *) * (f_count - position));
	}

// save the object pointer in the list
	f_data[position] = const_cast<moBase *>(object);
	f_data[position]->AddRef();
	f_count++;
}


/** \brief Append an object to this list.
 *
 * This function appends the specified \p object at the end of this
 * list.
 *
 * \param[in] object The object to append.
 *
 * \return This function always returns true.
 */
bool moList::Insert(const moBase *object)
{
	// insert the object at the end of the list
	moList::InsertAt(object, NO_POSITION);
	return true;
}


/** \brief Insert an object at the specified position.
 *
 * This function inserts the specified \p object at the specified
 * \p position.
 *
 * \bug
 * The list objects accept null pointers. But this can generate bugs
 * when looping through the list of items later or comparing between
 * different objects.
 *
 * \param[in] object	The object to insert.
 * \param[in] position	The position at which the object is to be
 * 			inserted or moList::NO_POSITION.
 *
 * \return This function always returns true.
 */
bool moList::Insert(const moBase *object, position_t position)
{
	// insert the object at that position
	moList::InsertAt(object, position);
	return true;
}


/** \brief Insert an object in this list.
 *
 * This function appends the \p object specified as a parameter to
 * this list unless it already exists.
 *
 * It actually calls Insert(const moBase *object, position_t position)
 * with the position set to moList::NO_POSITION.
 *
 * \bug
 * The list objects accept null pointers. But this can generate bugs
 * when looping through the list of items later or comparing between
 * different objects.
 *
 * \param[in] object The object to insert in this list.
 *
 * \return true if the item was added; false otherwise.
 */
bool moListUnique::Insert(const moBase *object)
{
	// calls the other Insert() function
	// which checks for the existance of the item first
	return Insert(object, NO_POSITION);
}


/** \brief Insert an object in this list.
 *
 * This function checks whether the \p object specified as a parameter
 * exists in the list. If so, then the function returns immediately
 * with false.
 *
 * Otherwise, the object is added to the list at the specified \p position.
 *
 * \bug
 * The list objects accept null pointers. But this can generate bugs
 * when looping through the list of items later or comparing between
 * different objects.
 *
 * \param[in] object The object to add to this list.
 * \param[in] position The position at which the object is to be added
 *			or moList::NO_POSITION to append the object.
 *
 * \return true if the object was added.
 */
bool moListUnique::Insert(const moBase *object, position_t position)
{
	if(!Exists(object)) {		// insert only if not found
		moList::InsertAt(object, position);
		return true;
	}

	return false;
}


/** \brief Insert an object in this sorted list.
 *
 * This function searches the the best position to insert the specified
 * object in this sorted list using the Find() function.
 *
 * The object is then added at that position.
 *
 * \bug
 * The list objects accept null pointers. But this can generate bugs
 * when looping through the list of items later or comparing between
 * different objects.
 *
 * \param[in] object The object to add to this sorted list.
 *
 * \return This function always returns true.
 */
bool moSortedList::Insert(const moBase *object)
{
 	Find(object);			// define f_closest
	InsertAt(object, f_closest);	// insert the object now
	f_closest = NO_POSITION;
	return true;
}


/** \brief Insert an object in this sorted list.
 *
 * This function searches the the best position to insert the specified
 * object in this sorted list using the Find() function.
 *
 * If an object that is equal is found in this list, then the object
 * specified as parameter is not added to the list and the function
 * returns false.
 *
 * Otherwise, the object is added at that position.
 *
 * \bug
 * The list objects accept null pointers. But this can generate bugs
 * when looping through the list of items later or comparing between
 * different objects.
 *
 * \param[in] object The object to add to this sorted list.
 *
 * \return true when the object was added to the list; false otherwise.
 */
bool moSortedListUnique::Insert(const moBase *object)
{
	if(!Exists(object)) {		// define f_closest
		InsertAt(object, f_closest);	// insert only if not found
		f_closest = NO_POSITION;
		return true;
	}
	f_closest = NO_POSITION;
	return false;
}




/** \brief Append a list to another.
 *
 * This function appends all the items of the input \p list to this
 * list starting by adding in this list at \p position.
 *
 * If you need a better granularity in the source list, you will have
 * to insert the items by hand instead.
 */
void moList::Append(const moListBase *list, position_t position)
{
	position_t	p;

// anything to append?
	if(list == 0 || list->Count() == 0) {
		return;
	}

// append item?
	if(position > f_count) {
		position = f_count;
	}

// enough room?
	if(f_count + list->Count() > f_maximum) {
		SetArraySize(f_count + list->Count());
	}

// need to move data?
	if(position < f_count) {
		memmove(f_data + position + list->Count(), f_data + position,
				sizeof(moBase *) * (f_count - position));
	}

// copy the other list object pointers
	memcpy(f_data + position, list->Array(),			/* Flawfinder: ignore */
				sizeof(moBase *) * list->Count());

// add the number of objects we just added in the list
	f_count += list->Count();

// reference these objects in this list
	p = list->Count();
	while(p > 0UL) {
		p--;
		f_data[position + p]->AddRef();
	}
}


/** \brief Append a list to another.
 *
 * This function appends the items of the specified \p list parameter
 * to this list.
 *
 * \param[in] list The list to append to this list.
 */
void moListBase::Append(const moListBase *list)
{
	position_t	i, max;

// anything to insert?
	if(list == 0) {
		return;
	}

// insert items one by one using the Insert function
	max = list->Count();
	for(i = 0; i < max; ++i) {
		Insert(list->Get(i));
	}
}



/** \brief Append the elements of a list to another.
 *
 * This function appends to this list all the elements of the input
 * \p list that do not match (are not equal) an items that exists
 * in this list. Note that if the input \p list has some items
 * duplicated, only one instance will be kept in this list.
 *
 * The \p position specifies where the items are added in this
 * list. It starts at that position.
 *
 * \param[in] list The list to be added to this list.
 * \param[in] position The start position where items are added in this list.
 * 			Set to moList::NO_POSITION to append the items at the
 * 			end of this list.
 */
void moListUnique::Append(const moListBase *list, position_t position)
{
	position_t	i, max;
	unsigned long	offset;

// anything to insert?
	if(list == 0) {
		return;
	}

// insert items one by one using the Insert function
	if(position > f_count) {
		position = f_count;
	}
	offset = f_count - position;	// used to always insert at the same place
	max = list->Count();
	i = 0;
	while(i < max) {
		Insert(list->Get(i), f_count - offset);
		i++;
	}
}


/** \brief Append a list at the end of this list.
 *
 * This function appends a list at the end of this list.
 *
 * \param[in] list The list to append to the other.
 */
void moList::Append(const moListBase *list)
{
	Append(list, NO_POSITION);
}



/** \brief Append a list at the end of this list.
 *
 * This function appends the item of the specified \p list
 * that are not already in this list at the end of this list.
 *
 * Note that if \p list has items duplicated, only one instance
 * will be kept in this list.
 *
 * \param[in] list Add the items of this parameter to this list.
 */
void moListUnique::Append(const moListBase *list)
{
	moListBase::Append(list);
}




/** \brief Delete an item in a list.
 *
 * This function deletes the item at the specified position.
 *
 * The item Release() function is automatically called.
 * 
 * An invalid position will be silently ignored. This is
 * because in the insertion functions, a position out of
 * bounds is used to append items.
 *
 * \note
 * The buffer holding the list of objects is never reduced
 * by this function.
 *
 * \param[in] position The position of the item to be deleted.
 *
 * \sa Empty()
 * \sa SetSize()
 */
void moListBase::Delete(position_t position)
{
	if(position >= f_count) {
		return;
	}

	f_data[position]->Release();

	f_count--;
	memmove(f_data + position, f_data + position + 1,
			(f_count - position) * sizeof(moBase *));
}



/** \brief Set the size of the buffer (Count() will then return that size).
 *
 * This function will change the number of items present in a list.
 * The Count() function will then return this value.
 * 
 * If necessary, the array will be enlarged in order to accept the
 * request.
 * 
 * When a list is made larger, new entries are cleared (null pointers).
 * 
 * When a list is made smaller, the last items are lost. The function
 * will automatically called the Release() function of all the items
 * which are lost.
 * 
 * \exception moError(MO_ERROR_OVERFLOW)
 * An moError() will be thrown when \p size is set larger than
 * MAXIMUM_SIZE.
 *
 * \exception std::bad_alloc
 * An std::bad_alloc exception is thrown whenever
 * the required memory buffer cannot be allocated.
 *
 * \bug
 * Sorted lists may not like to have entries which are zero
 * pointers. It isn't wise to use this function on sorted lists.
 * 
 * \sa SetArraySize()
 * \sa Insert()
 */
void moListBase::SetSize(unsigned long size)
{
	if(size <= f_count) {
		while(size < f_count) {
			f_count--;
			f_data[f_count]->Release();
		}
		// the following may shrink the array in memory!
		SetArraySize(size);
	}
	else if(size > MAXIMUM_SIZE) {
		throw moError(MO_ERROR_OVERFLOW,
"moListBase: size too large (accepts a maximum of %lu entries; %lu was specified)",
						MAXIMUM_SIZE, size);
	}
	else {
		// ensure it will fit
		SetArraySize(size);
		// clear the extra entries
		memset(f_data + f_count, 0, (size - f_count) * sizeof(moBase *));
		// set the new size
		f_count = size;
	}
}




/** \brief Define the offset of a compare function.
 *
 * Changes the default Compare() function call to this
 * one. This order_function parameter is an offset to one
 * of the compare functions of the items inserted in this
 * list.
 *
 * This is primarily used to sort the same set of objects
 * with different criterias in several different lists.
 *
 * \param[in] order_function The offset to the new comparison function
 *
 * \bug
 * A comparison function can only be set on an empty list.
 * It is a major error to try to change the compare function
 * of a non-empty list and for this reason this function
 * will throw an moBug with MO_ERROR_COMPARE error if it
 * ever happens.
 */
void moSortedList::SetCompare(compare_function_t order_function)
{
	if(f_count > 0UL) {
		// Okay... We could have tested with f_count > 1 instead... but still!
		throw moBug(MO_ERROR_COMPARE, "cannot change list comparison function in a non-empty list.");
	}

	f_order_function = order_function;
}


/** \brief Initializes a list.
 *
 * This function initializes the base class of all the available lists.
 */
moListBase::moListBase(void)
{
	//f_maximum = 0; -- auto-init
	//f_count = 0; -- auto-init
	f_data = 0;
	f_last_found = NO_POSITION;
}

/** \brief Copy a list.
 *
 * This function copies the list passed as a parameter in this list.
 *
 * \note
 * Since this is a constructor, all the virtual functions cannot be
 * used. If they are necessary for some lists, this operator needs
 * to be overloaded (i.e. sorted lists.)
 *
 * \param[in] list The list to copy.
 */
moListBase::moListBase(const moListBase& list)
	: moBase(list)
{
	position_t	pos;
	moBase		* const *src;

	//f_maximum = 0; -- auto-init
	//f_count = 0; -- auto-init
	f_data = 0;
	f_last_found = NO_POSITION;

	// This was SetArraySize() when we wanted to call Insert()
	// but Insert() cannot be called so we do it this way instead.
	SetSize(list.f_count);

	// Since we now loop over all the items anyway, we can just
	// also copy them, it will certainly be faster!
	//memcpy(f_data, list.Array(), sizeof(moBase *) * f_count);

	src = list.f_data;
	pos = f_count;
	while(pos > 0UL) {
		pos--;
		f_data[pos] = src[pos];
		f_data[pos]->AddRef();
	}

#if 0
// This is the correct way to do things, but the problem is
// that the Insert() function is not yet valid here!
	src = list.f_data;
	max = list.f_count;
	for(pos = 0; pos < max; pos++) {
		Insert(src[pos]);
	}
#endif
}

/** \brief Clean up the list.
 *
 * This function ensures that the list is empty and quit.
 */
moListBase::~moListBase()
{
	Empty();	// <- this is virtual and it may have an effect!

 	// see comment in the SetArraySize() function about the free()
 	mo_free(f_data);
}


/** \brief Assign a copy of a list in another.
 *
 * This function copies the input list in this list.
 *
 * \param[in] list The list to copy in this list.
 *
 * \return A reference to this list
 */
moListBase& moListBase::operator = (const moListBase& list)
{
	position_t	pos, max;

	if(this == &list) {
		return *this;
	}

	Empty();
	max = list.f_count;
	for(pos = 0; pos < max; pos++) {
		Insert(list.f_data[pos]);
	}

	return *this;
}


/** \brief Initializes a list.
 *
 * This function initializes the list.
 */
moList::moList(void)
{
}


/** \brief Copy a list.
 *
 * This function copies the input list in this list in an
 * effective way.
 *
 * \param[in] list The list to copy in this list.
 */
moList::moList(const moListBase& list)
	: moListBase()
{
	Append(&list);
}


/** \brief Clean up the list.
 *
 * This function ensures the list is empty.
 */
moList::~moList()
{
}


/** \brief Initializes a list.
 *
 * This function initializes the list of unique item.
 */
moListUnique::moListUnique(void)
{
}


/** \brief Copy a list.
 *
 * This function copies the input list in this list in an efficient way.
 *
 * \param[in] list The input list to copy in this list.
 */
moListUnique::moListUnique(const moListBase& list)
	: moList()
{
	Append(&list);
}


/** \brief Clean up a list.
 *
 * This function cleans up the unique list.
 */
moListUnique::~moListUnique()
{
}


/** \brief Initializes a list.
 *
 * This function initializes a sorted list. By default, the
 * list will use the standard Compare() function to order its
 * items.
 */
moSortedList::moSortedList(void)
{
	f_closest = NO_POSITION;
	f_order_function = 0;
}


/** \brief Copy a list.
 *
 * This function copies the input list in this list.
 *
 * The new list will be sorted using the default Compare()
 * function and not the input list comparison function whether
 * or not it was changed with SetCompare().
 *
 * Duplicates are kept in the new list.
 *
 * \param[in] list The input list to copy in this list
 */
moSortedList::moSortedList(const moListBase& list)
	: moListBase()
{
	f_closest = NO_POSITION;
	f_order_function = 0;
	Append(&list);
}


/** \brief Clean up a list.
 *
 * This function ensures that the sorted list is empty.
 */
moSortedList::~moSortedList()
{
}


/** \brief Copy a list.
 *
 * This function copies the input list in this list.
 *
 * This function is an overload of the base list assignment
 * operator to ensure that the items are inserted at the
 * right place (using the comparison function to order them.)
 *
 * The previous content of this list is lost.
 *
 * \param[in] list The list to copy in this list.
 *
 * \return A reference to this list
 */
moSortedList& moSortedList::operator = (const moListBase& list)
{
	if(this == &list) {
		return *this;
	}

	Empty();

	f_closest = NO_POSITION;
	f_order_function = 0;
	Append(&list);

	return *this;
}


/** \brief Initializes a list.
 *
 * This function initializes this list.
 */
moSortedListUnique::moSortedListUnique(void)
{
}


/** \brief Copy a list.
 *
 * This function copies the specified list to this list.
 *
 * This list will be ordered and duplicates will not be
 * copied.
 *
 * \param[in] list The list to copy in this list.
 */
moSortedListUnique::moSortedListUnique(const moListBase& list)
	: moSortedList()
{
	Append(&list);
}


/** \brief Clean up a list.
 *
 * This function ensures that this list is empty.
 */
moSortedListUnique::~moSortedListUnique()
{
}


/** \brief Check whether the list is sorted.
 *
 * This function returns true if the list is sorted and false otherwise.
 *
 * moSortedList and moSortedListUnique will return true. Other lists
 * return false.
 *
 * \bug
 * If items change and these changes alter their order as defined
 * by their Compare() function, the list coherency will be lost and
 * even though this function may return true, it won't be anymore.
 *
 * \return true if the items in the list are sorted.
 */
bool moListBase::IsSorted(void) const
{
	return false;
}


// inherit the definition from moListBase
bool moSortedList::IsSorted(void) const
{
	return true;
}


/** \brief Test whether a list accepts duplicates.
 *
 * This function returns true if the list will never have
 * duplicated objects (i.e. was created with moListUnique
 * or moSortedListUnique).
 *
 * \return true if equal items cannot be duplicated
 *
 * \bug
 * If items change and these changes make them equal, the list
 * coherency will be lost and even though this function may return
 * true, it won't be anymore.
 */
bool moListBase::IsUnique(void) const
{
	return false;
}


// inherit the definition from moListBase
bool moListUnique::IsUnique(void) const
{
	return true;
}


// inherit the definition from moListBase
bool moSortedListUnique::IsUnique(void) const
{
	return true;
}


/** \brief Test whether a list if empty
 *
 * The IsEmpty() function returns true if the given list is
 * empty. It is equivalent to testing whether the Count() is
 * zero.
 *
 * \return true when the list is empty; false otherwise
 *
 * \sa Count()
 */
bool moListBase::IsEmpty(void) const
{
	return this == 0 ? true : f_count == 0UL;
}


/** \brief Make a list empty.
 *
 * This function makes this list empty.
 *
 * If the list was not empty, the Release() function of any item
 * still defined in the list is called. This may have the side
 * effect of deleting the item if the counter reaches zero.
 */
void moListBase::Empty(void)
{
	if(this != 0) while(f_count > 0UL) {
		f_count--;
		f_data[f_count]->Release();
	}
}


/** \brief Return the number of items present in this list.
 *
 * This function returns the number of items currently defined
 * within this list.
 *
 * \return The number of items in the list.
 *
 * \sa IsEmpty()
 * \sa Array()
 */
unsigned long moListBase::Count(void) const
{
	return this == 0 ? 0 : static_cast<unsigned long>(f_count);
}


/** \brief Maximum number of comparison before Find() returns.
 * 
 * This function returns the number of items that the system will
 * compare before returning from a Find() function call.
 * 
 * This assumes the item being searched is unique. If not, then
 * additional compares will occur to find the first instance of
 * equal items.
 * 
 * Lists that are not sorted require the Find() to search all the
 * items sequentially. In this case, this function returns the
 * same value as Count().
 *
 * \note
 * The Insert() and Exists() functions call the Find() function.
 *
 * \return the maximum number of compares which will occur in a Find()
 *
 * \sa Exists()
 * \sa Count()
 * \sa IsSorted()
 */
unsigned long moListBase::MaxCompare(void) const
{
	if(this == 0) {
		return 0;
	}
	if(IsSorted() && f_count >= 4UL) {
		return static_cast<unsigned long>(ceil(log(static_cast<double>(static_cast<unsigned long>(f_count))) / log(2.0)));
	}
	return static_cast<unsigned long>(f_count);
}


/** \brief Returns the array buffer pointer.
 *
 * This function returns a pointer to the internal array of
 * pointers used to maintain the list of items.
 *
 * It is unwise to try to modify the buffer. The number of
 * items in the buffer is as returned by Count(). This buffer
 * should only be used as a read-only buffer. This pointer
 * will change if other functions are called.
 *
 * Note that you are free to modify the objects available in
 * this array, but not the array itself.
 *
 * \return a pointer to the array of object;
 *	the pointer can be zero (0) if the list is empty
 *
 * \sa Count()
 * \sa Append()
 * \sa SetArraySize()
 *
 * \bug
 * This function should not exists, yet it simplifies the
 * Append() function so it is possible to copy a list in
 * another very quickly.
 *
 * \p
 * The pointer will be invalidated if other functions are
 * called on this list (any function that calls the
 * SetArraySize() to be precise).
 */
moBase * const * moListBase::Array(void) const
{
	return f_data;
}


/** \brief Checks whether a given object is present in a list.
 *
 * The Exists() function searches for object parameter and returns
 * true if it finds it; false otherwise.
 *
 * \param[in] object An object to search in the given list.
 *
 * \return true when the given object is present in the list.
 */
bool moListBase::Exists(const moBase *object) const
{
	return Find(object) != NO_POSITION;
}


/** \brief Append an item or merge two lists.
 *
 * Append the specified object in this list.
 *
 * This function uses the Insert() function to insert the object
 * appropriately.
 *
 * \sa Insert()
 * \sa Append()
 */
moListBase& moListBase::operator += (const moBase& object)
{
	Insert(&object);

	return *this;
}


/** \brief Append an item or merge two lists.
 *
 * This function appends all of the items present in the list
 * specified as a parameter to this list. Note that
 * the items are appended in the order they are found in the
 * list parameter. That is significant only if the destination
 * list is not sorted. It may be faster to call this function when
 * the destination list is not sorted than to add items one by
 * one.
 *
 * This function uses the Append() function.
 *
 * IMPORTANT: To insert a list as an object of another list with
 * the += operator, you need to cast the list into an moBase object
 * to avoid calling the second instance of the function:
 * 
 * \code
 * // call the other += operator (not this one)
 * list1 += dynamic_cast<const moBase&>(list2);
 * \endcode
 * 
 * It may also be wise (clearer, safer, etc.) to use the Insert()
 * function in this case:
 * 
 * \code
 * list1->Insert(list2);
 * \endcode
 *
 * \sa Insert()
 * \sa Append()
 */
moListBase& moListBase::operator += (const moListBase& list)
{
	Append(&list);

	return *this;
}








/** \brief Get the item at the specified position.
 *
 * This function retrieves the item at the specified position
 * and return a pointer to it.
 *
 * \exception moError(MO_ERROR_OVERFLOW)
 * If the position is out of bounds, this exception is thrown.
 * If the list is empty, this exception will always be raised.
 *
 * \param[in] position The position of the item to retrieve.
 *
 * \return A pointer to the item being retrieved, may be NULL.
 */
moBase *moListBase::Get(position_t position) const
{
	if(position >= f_count) {
		throw moError(MO_ERROR_OVERFLOW, "moListBase: position out of bounds");
	}

	return const_cast<moBase *>(f_data[position]);
}


/** \brief Get the very first item.
 *
 * This function retrieves the first item of the list.
 *
 * \exception moError(MO_ERROR_OVERFLOW)
 * This exception is raised if the list is empty.
 *
 * \return A pointer to the first item, may be NULL.
 */
moBase *moListBase::GetFirst(void) const
{
	return Get(0);
}


/** \brief Get the very last item.
 *
 * This function retrieves the last item of the list.
 *
 * \exception moError(MO_ERROR_OVERFLOW)
 * This exception is raised if the list is empty.
 *
 * \return A pointer to the last item, may be NULL.
 */
moBase *moListBase::GetLast(void) const
{
	return Get(f_count - 1);
}


/** \brief Get the specified item by copy.
 *
 * This function retrieves the specified item by copy instead
 * of a pointer to the instance defined in the list.
 *
 * The function will generate a processor exception if the
 * object at the specified position is a NULL pointer.
 *
 * \exception moError(MO_ERROR_OVERFLOW)
 * This exception is raised if the position is out of bounds
 * or the list is empty.
 *
 * \return A copy of the object at the specified position.
 */
moBase moListBase::GetCopy(position_t position) const
{
	if(position >= f_count) {
		throw moError(MO_ERROR_OVERFLOW, "moListBase: position out of bounds");
	}

	return *f_data[position];
}


/** \brief Set an option at the specified position.
 *
 * This function sets the object at the specified position.
 *
 * This function is not available in sorted lists.
 *
 * The position can be out of range in which case the list will
 * be grown. If intermediate positions have not been defined yet,
 * they are set to NULL.
 *
 * The object passed as an input parameter will have its
 * AddRef() function called.
 *
 * The input object pointer can be NULL.
 *
 * \exception moError(MO_ERROR_OVERFLOW)
 * If the position is negative or too large this exception is
 * thrown. See SetSize().
 *
 * \sa SetSize()
 * \sa moBase::AddRef()
 *
 * \param[in] position The position at which the item is to be inserted.
 * \param[in] object The object to save at that position.
 */
void moList::Set(position_t position, const moBase *object)
{
	if(position >= f_count) {
		SetSize(position + 1);
	}

	object->AddRef();
	f_data[position] = const_cast<moBase *>(object);
}


/** \brief Set an object at the specified position.
 *
 * This function sets the specified object at the specified
 * postion unless another object in the list is equal to
 * that new specified object.
 *
 * \bug
 * At this time, it is not possible to change the object at
 * the specified position if that object is equal to the object
 * specified as parameter. (i.e. if list[position] == object, the
 * function does NOT replace the object specified at list[position].)
 *
 * \param[in] position The position at which the item is to be inserted.
 * \param[in] object The object to save at that position if unique.
 */
void moListUnique::Set(position_t position, const moBase *object)
{
	if(!Exists(object)) {
		moList::Set(position, object);
	}
}






/** \class moList::moListItem
 *
 * \brief A reference to a list item.
 *
 * Whenever you use the read/write [] operator, you get an moListItem
 * object in return. This object lets you read and write in the
 * referenced object.
 *
 * In general, this will be used in expressions such as the following
 * which means that you do not need to know about this class.
 *
 * \code
 * moList list;
 * ...
 * list[3]->DoSomething();
 * \endcode
 *
 * \bug
 * Like any reference, it has a limited life time. The reference is very
 * likely to be pointing at the wrong item after an Insert(), Append()
 * or Delete() is called.
 */


/** \brief Get a read/write reference to a list item.
 *
 * This function returns an moListItem object that can be used
 * to read and write the item at the specified index.
 *
 * \bug
 * In sorted lists, modifying the sort key of the object may
 * result in a non-sorted list breaking the contract of the
 * sorted list class.
 *
 * \param[in] index The index of the object to reference.
 *
 * \return An moListItem object.
 */
moList::moListItem moList::operator [] (int index)
{
	return moListItem(this, static_cast<position_t>(index));
}



/** \brief Get a read-only reference to a list item.
 *
 * This function returns a constant reference to an object
 * defined in the liast at the specified index.
 *
 * \bug
 * This function will return a NULL reference if the object
 * pointer can be NULL in the list. It is your responsibility
 * to ensure that this isn't the case if you need to access
 * the object.
 *
 * \param[in] index The index of the object to reference.
 *
 * \return A reference to the object.
 */
const moBase& moListBase::operator [] (int index) const
{
	return *Get(static_cast<position_t>(index));
}






/** \brief Initializes an moListItem object.
 *
 * This function initializes an moListItem with a pointer back
 * to the list being referenced and a position in that list.
 *
 * This is used by the read/write [] operator.
 *
 * \param[in] list The list being referenced.
 * \param[in] position The item of that list being referenced.
 */
moList::moListItem::moListItem(const moList *list, position_t position)
	: f_list(list),
	  f_position(position)
{
}


/** \brief Copies an moListItem in another.
 *
 * This function copies the input moListItem in this one.
 *
 * \param[in] list_item The input list item to copy in this one.
 */
moList::moListItem::moListItem(const moListItem& list_item)
	: f_list(list_item.f_list),
	  f_position(list_item.f_position)
{
}


/** \brief Set the item in the list.
 *
 * This function sets the item in the referenced list of the
 * moListItem at the position specified in the constructor.
 *
 * \param[in] object The object to set in the list.
 *
 * \return A reference to this moListItem object.
 */
moList::moListItem& moList::moListItem::operator = (const moBase& object)
{
	f_list->Set(f_position, &object);

	return *this;
}


/** \brief Retrieve a read/write object reference of the list object.
 *
 * This function retrieves a reference to the list object and
 * returns it.
 *
 * \bug
 * Note that this function can return a reference with a NULL
 * pointer. It is your responsibility to check for that case.
 *
 * \return A reference to the list object.
 */
moList::moListItem::operator moBase& (void)
{
	return *f_list->Get(f_position);
}


/** \brief Retrieve a read-only object reference of the list object.
 *
 * This function retrieves a reference to the list object and
 * returns it. The reference is read-only.
 *
 * \bug
 * Note that this function can return a reference with a NULL
 * pointer. It is your responsibility to check for that case.
 *
 * \return A reference to the list object.
 */
moList::moListItem::operator const moBase& (void) const
{
	return const_cast<const moBase&>(*f_list->Get(f_position));
}




};			// namespace molib;

// vim: ts=8

