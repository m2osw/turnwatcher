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



#ifndef MO_LIST_H
#define	MO_LIST_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_ERROR_H
#include	"mo_error.h"
#endif


namespace molib {

/** The base class for all the lists.
 * This is the base class used by all the other list objects.
 * You should not have to derive from this class unless you are
 * creating a new version of moList...
 */

class MO_DLL_EXPORT moListBase : public moBase
{
public:
	/** \brief The position of an item in an moList.
	 *
	 * All lists use integers of type position_t to reference objects by index.
	 */
#ifdef _MSC_VER
	// TODO: mo_controlled.h presents some problems under VC++
	//
	typedef uint32_t	position_t;
#else
	typedef ruint32_t	position_t;
#endif

	/// a value representing an invalid position, the end of the list or not found
	static const uint32_t		NO_POSITION  = static_cast<uint32_t>(-1);
	/// a maximum size so that parameter can be tested once in a while
	static const unsigned long	MAXIMUM_SIZE = 1000 * 1000;

				moListBase(void);
				moListBase(const moListBase& list);
	virtual			~moListBase();

	virtual const char *	moGetClassName(void) const;

	moListBase&		operator = (const moListBase& list);

	virtual compare_t	Compare(const moBase& object) const;
	virtual bool		IsSorted(void) const;
	virtual bool		IsUnique(void) const;
	bool			IsEmpty(void) const;
	virtual void		Empty(void);
	bool			Exists(const moBase *object) const;
	unsigned long		Count(void) const;
	moBase * const *	Array(void) const;
	unsigned long		MaxCompare(void) const;

	/** \brief Find the first item in the list that matches \p object.
	 *
	 * This function searches for the first item in the list that
	 * matches \p object.
	 *
	 * \param[in] object   The object to search in the list
	 *
	 * \return The position where the object was found or NO_POSITION.
	 */
	virtual position_t	Find(const moBase *object) const = 0;
	virtual position_t	FindNext(void) const;
	virtual void		Append(const moListBase *list);

	/** \brief Insert \p object into this list
	 *
	 * This function inserts the specified object in this list.
	 *
	 * In case of a sorted list, the object is placed so as to keep
	 * the list sorted.
	 *
	 * In case of non-sorted list, it is added at the end of the list.
	 *
	 * In case of Unique lists, the item is inserted only if the list
	 * does not already include such an item.
	 *
	 * \param[in] object   The pointer to the object to insert
	 *
	 * \return true if the item was inserted
	 */
	virtual bool		Insert(const moBase *object) = 0;
	void			Delete(position_t position);

	void			SetArraySize(unsigned long size);
	void			SetSize(unsigned long size);
	moBase *		Get(position_t position) const;
	moBase *		GetFirst(void) const;
	moBase *		GetLast(void) const;
	moBase			GetCopy(position_t position) const;
	moListBase&		operator += (const moBase& object);
	moListBase&		operator += (const moListBase& list);

	/** \brief Append a list or an item to this list.
	 *
	 * This function attempts to append a list or an item to this list.
	 * The input is a smart pointer that will be converted to an moListBase
	 * object or an moBase object.
	 *
	 * \exception moError
	 * If the object is not an moListBase nor an moBase object, the
	 * function throws an error.
	 *
	 * \param[in] input   A list or an item smart pointer.
	 *
	 * \return A reference to this list.
	 */
	template<class C>
	moListBase&		operator += (const moSmartPtr<C>& input)
				{
					const moListBase *l = dynamic_cast<const moListBase *>(static_cast<const C *>(input));
					if(l == 0) {
						const moBase *b = dynamic_cast<const moBase *>(static_cast<const C *>(input));
						if(b == 0) {
							throw moError("To a list, you cannot add an object which is not an moBase object or an moListBase");
						}
						Insert(b);
					}
					else {
						Append(l);
					}

					return *this;
				}

	const moBase&		operator [] (int index) const;

protected:
	void			InsertAt(const moBase *object, position_t position);

	/// Maximum number of entries before a realloc() call.
	zuint32_t		f_maximum;
	/// Current number of entries (i.e. Count() result.)
	zuint32_t		f_count;
	/// An array of object pointers. The list manages the reference count of these objects.
	moBase **		f_data;
	/// The last object found or closest position.
	mutable position_t	f_last_found;
};




class moList;
typedef moSmartPtr<moList>	moListSPtr;

class MO_DLL_EXPORT moList : public moListBase
{
public:
	// an object to access read-write items with []
	// note that moListItem can't inherit moBase
	class moListItem
	{
	public:
					moListItem(const moList *list, position_t position);
					moListItem(const moListItem& list_item);

		moListItem&		operator = (const moBase& object);
					operator moBase& (void);
					operator const moBase& (void) const;

	private:
		/// Do not allow copies of list items; it could be confused with setting a value in the item.
		moListItem&		operator = (const moListItem& list_item) { return *this; }
		/// The list being referenced.
		moListSPtr		f_list;
		/// The position of the item in the list. Note that if the list changes then this position can become invalid.
		const position_t	f_position;
	};

				moList(void);
				moList(const moListBase& list);
	virtual			~moList();

	virtual const char *	moGetClassName(void) const;

	virtual position_t	Find(const moBase *object) const;
	virtual void		Append(const moListBase *list);
	virtual bool		Insert(const moBase *object);
	virtual void		Append(const moListBase *list, position_t position);
	virtual bool		Insert(const moBase *object, position_t position);
	virtual void		Set(position_t position, const moBase *object);
	moListItem		operator [] (int index);
};


class MO_DLL_EXPORT moListUnique : public moList
{
public:
				moListUnique(void);
				moListUnique(const moListBase& list);
	virtual			~moListUnique();

	virtual const char *	moGetClassName(void) const;

	virtual bool		IsUnique(void) const;
	virtual void		Append(const moListBase *list);
	virtual bool		Insert(const moBase *object);
	virtual void		Append(const moListBase *list, position_t position);
	virtual bool		Insert(const moBase *object, position_t position);
	virtual void		Set(position_t position, const moBase *object);
};

typedef moSmartPtr<moListUnique>	moListUniqueSPtr;


class MO_DLL_EXPORT moSortedList : public moListBase
{
public:
				moSortedList(void);
				moSortedList(const moListBase& list);
	virtual			~moSortedList();

	moSortedList&		operator = (const moListBase& list);

	virtual const char *	moGetClassName(void) const;

	void			SetCompare(compare_function_t order_func);
	virtual bool		IsSorted(void) const;
	virtual position_t	Find(const moBase *object) const;
	virtual position_t	FindNext(void) const;

	virtual bool		Insert(const moBase *object);

protected:
	/// If Find() does not find an exact match, this represents the closest object found
	mutable position_t	f_closest;
	/// Function used to order elements added to this list; use Compare() by default
	compare_function_t	f_order_function;
};

typedef moSmartPtr<moSortedList>	moSortedListSPtr;


class MO_DLL_EXPORT moSortedListUnique : public moSortedList
{
public:
				moSortedListUnique(void);
				moSortedListUnique(const moListBase& list);
	virtual			~moSortedListUnique();

	virtual const char *	moGetClassName(void) const;

	virtual bool		IsUnique(void) const;
	virtual bool		Insert(const moBase *object);
};

typedef moSmartPtr<moSortedListUnique>	moSortedListUniqueSPtr;







/** \brief A template to create lists of a given type
 *
 * When you create a list of one specific type of objects, you can use this template.
 * For instance, to create a list of strings you write:
 *
 * \code
 * typedef moTmplList<moWCString, moList> moListOfWCStrings;
 * \endcode
 *
 * Now you can use the Get() operator and the template will take care of casting the
 * elements to moWCString pointers. Note that template doesn't prevent you from adding
 * different type of elements to the list. This is still your reponsability.
 */
template<class T, class L>
class MO_DLL_EXPORT_TMPL moTmplList : public L
{
public:
	/// Ensure that all virtual destructors are called.
	virtual			~moTmplList() {}
	/// Like moListBase::Get() but return the proper object type.
	T *			Get(moListBase::position_t position) const { return dynamic_cast<T *>(L::Get(position)); }
	/// Like moListBase::GetFirst() but return the proper object type.
	T *			GetFirst(void) const { return dynamic_cast<T *>(L::GetFirst()); }
	/// Like moListBase::GetLast() but return the proper object type.
	T *			GetLast(void) const { return dynamic_cast<T *>(L::GetLast()); }
#ifndef _MSC_VER
	//
	// This is a problem under VC++ for abstract base classes
	// Looks like we'll have to just copy "by hand."
	//
	/// Like moListBase::GetCopy() but return the proper object type.
	T			GetCopy(moListBase::position_t position) const { return dynamic_cast<T>(L::GetCopy(position)); }
#endif
	/// Like moListBase::operator = () but return the proper list type.
	L&			operator = (const moListBase& list) { return dynamic_cast<L&>(L::operator = (list)); }
	/// Like moListBase::operator[] () but return the proper object type.
	const T&		operator [] (int index) { return dynamic_cast<const T&>(L::operator [] (index)); }
	/// Like moListBase::operator[] () const but return the proper object type.
	const T&		operator [] (int index) const { return dynamic_cast<const T&>(L::operator [] (index)); }
};



};			// namespace molib;

// vim: ts=8 sw=8
#endif	// #ifndef MO_LIST_H

