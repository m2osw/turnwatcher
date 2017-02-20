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



#ifndef MO_PROPS_H
#define	MO_PROPS_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_THREAD_H
#include	"mo_thread.h"
#endif
#ifndef MO_BUFFER_H
#include	"mo_buffer.h"
#endif





namespace molib
{



// predefines, it is necessary for the signal handler and such
class MO_DLL_EXPORT moProp;
class MO_DLL_EXPORT moPropRef;
typedef moSmartPtr<moProp>	moPropSPtr;



// The user property signal handler; the function
// ValueChanged() is called each time a property is modified;
// it is called on that property only; at this time, the bag
// has no idea one of its property was modified
// IMPORTANT: the signal handlers are being called from the
// thread which modifies the property set
class MO_DLL_EXPORT moPropSignalHandler : public moBase
{
public:
	virtual const char *	moGetClassName(void) const;

	// NOTE: I put a body in case we decide to add more
	// functions since then some people may not want to
	// overload all the functions! This function could
	// otherwise be pure virtual.
	virtual void		SignalValueChanged(moPropRef& prop, moBase *data);

	// Will we ever need to "auto-copy" handlers when the
	// type of a property changes?
	//virtual bool		SignalAutoCopy(void) const;
};

typedef moSmartPtr<moPropSignalHandler>		moPropSignalHandlerSPtr;




// The following is similar to the prop signal handler however
// it will be used whenever you need to be signalled in your
// own thread. This can in many cases be quite important.
class MO_DLL_EXPORT moPropEvent : public moEvent
{
public:
				moPropEvent(const moProp& prop, moBase *data, const moEventDispatcher *dispatcher);
				moPropEvent(const moPropEvent& event);
	virtual			~moPropEvent();

	virtual moEventSPtr	Duplicate(void) const;

	void			SetProperty(const moProp& prop);
	moPropSPtr		GetProperty(void) const;

	void			SetData(const moBase *prop);
	moBaseSPtr		GetData(void) const;

	void			SetDispatcher(const moEventDispatcher *dispatcher);
	moEventDispatcherSPtr	GetDispatcher(void) const;

private:
	moPropSPtr		f_prop;
	moBaseSPtr		f_data;
	moEventDispatcherSPtr	f_dispatcher;
};

typedef moTmplList<moPropEvent, moList>	moListOfPropEvent;
typedef moSmartPtr<moPropEvent>		moPropEventSPtr;



// All of the property objects (moProp, moPropBag, moPropSimple and
// moPropArray) are derived from this moProp object;
// All of these objects have their constructor protected so you can't
// create one of them without using a reference
class MO_DLL_EXPORT moProp : public moBase
{
public:
	// when enumerating a property bag we need to have a type
	enum prop_type_t {
		MO_PROP_TYPE_UNKNOWN = 0,	// invalid/error/any
		MO_PROP_TYPE_PROP_BAG,		// moPropBag
		MO_PROP_TYPE_INT,		// int
		MO_PROP_TYPE_LONG_LONG,		// long long
		MO_PROP_TYPE_FLOAT,		// float
		MO_PROP_TYPE_DOUBLE,		// double
		MO_PROP_TYPE_POINTER,		// pointer (an moBaseSPtr)
		MO_PROP_TYPE_STRING,		// moWCString
		MO_PROP_TYPE_BINARY,		// a void * + size_t pair
		MO_PROP_TYPE_ARRAY,		// an array of properties

		MO_PROP_TYPE_max	// end the enum
	};

				moProp(mo_name_t name);
				moProp(const moProp& prop);
	virtual			~moProp();

	virtual const char *	moGetClassName(void) const;

	void			Lock(void) const;
	void			Unlock(void) const;

	virtual void		Copy(const moProp& prop);
	virtual moPropSPtr	Duplicate(void) const = 0;

	mo_name_t		GetName(void) const { return this == 0 ? moNamePool::GetNamePool()["no-name"] : f_name; }
	virtual prop_type_t	GetType(void) const = 0;

	void			LockType(void);
	void			UnlockType(void);
	bool			IsTypeLocked(void) const { return f_lock_type; }

	// add either an moEventPipe or an moPropSignalHandler or both
	void			AddHandler(moBase& handler, const moBase *data = 0);
	void			RemoveHandler(moBase& handler);

	void			Signal(void);

	// always sorted by name
	virtual compare_t	Compare(const moBase& object) const;

protected:
	mutable moMutex		f_mutex;

private:
	class moHandler : public moBase
	{
	public:
					moHandler(const moBase& handler, const moBase *data);

		void			Call(moProp *prop);

		virtual compare_t	Compare(const moBase& object) const;

	private:
		moBaseSPtr		f_handler;
		moBaseSPtr		f_data;
	};
	typedef moSmartPtr<moHandler>		moHandlerSPtr;
	typedef moTmplList<moHandler, moList>	moListOfHandlers;

	const mo_name_t		f_name;
	mbool_t			f_lock_type;

	// because most properties don't have handlers, we may
	// want to consider changing the following with a pointer
	// to a list and a pointer to a mutex instead of class members
	mutable moMutex		f_signal_mutex;		// we use this mutex to lock the property while signalling
	bool			f_signal_list_changed;	// the same thread may change the list of signals while signalling
	moListOfHandlers	f_handlers;
};

//typedef moSmartPtr<moProp>	moPropSPtr; -- already declared


class MO_DLL_EXPORT moPropAutoLock
{
public:
				moPropAutoLock(const moProp& prop)
				{
					f_prop = &prop;
					if(f_prop) {
						prop.Lock();
					}
				}
				~moPropAutoLock()
				{
					if(f_prop) {
						f_prop->Unlock();
					}
				}

	void			Unlock(void)
				{
					if(f_prop) {
						f_prop->Unlock();
						f_prop = 0;
					}
				}

private:
	moPropSPtr		f_prop;
};




// This is the definition of a bag of properties; a bag can
// include as many properties as you'd like; it uses a Get/Set
// and Delete to manage the properties it knows of; the references
// will actually attach themselves to the properties of a property
// bag once you link your property reference with a bag; then you
// can use the reference as a direct view of the property
class MO_DLL_EXPORT moPropBag : public moProp
{
public:
	static const unsigned int	DUMP_FLAG_RECURSIVE = 0x0001;

	virtual const char *	moGetClassName(void) const;

	virtual void		Copy(const moProp& prop);
	virtual moPropSPtr	Duplicate(void) const;

	virtual prop_type_t	GetType(void) const { return MO_PROP_TYPE_PROP_BAG; }

	unsigned long		Count(void) const { return f_props.Count(); }
	void			Empty(void) { f_props.Empty(); }
	void			Dump(unsigned int flags = DUMP_FLAG_RECURSIVE, const char *message = 0) const;

	moPropSPtr		Get(int index_or_name) const;
	bool			Set(int index_or_name, const moProp& prop, bool overwrite = true);
	void			Delete(int index_or_name);

protected:
	friend class moPropBagRef;
				moPropBag(mo_name_t name);
				moPropBag(const moPropBag& bag, bool recursive = false);
	//virtual		~moPropBag(); -- no need

private:
	moPropBag&		operator = (const moPropBag& bag);

	void			DumpProps(unsigned int flags, unsigned int indent) const;
	void			DumpProp(unsigned int flags, unsigned int indent, moPropSPtr prop) const;

	typedef moTmplList<moProp, moSortedList>	moSortedListOfProps;
	moSortedListOfProps	f_props;	// list of moProp *
};

typedef moSmartPtr<moPropBag>	moPropBagSPtr;





// simple properties are integers, floating points, boolean, etc.
// these don't require much at all
template<class T, moProp::prop_type_t TYPE>
class MO_DLL_EXPORT moPropSimple : public moProp
{
public:
#if 0
	moPropSimple<T, TYPE>	moPropSimple(const moPropSimple<T, TYPE>& simple)
					: moProp(simple)
				{
					f_value = simple.f_value;
				}
#endif
				moPropSimple(mo_name_t name)
					: moProp(name)
				{
				}
	virtual			~moPropSimple()
				{
				}

	const char *		moGetClassName(void) const
				{
					// unfortunatly T and TYPE cannot be converted to a string in that message
					return "molib::moBase::moProp::moPropSimple<class T, moProp::prop_type_t TYPE>";
				}


	virtual void		Copy(const moProp& prop)
				{
					if(this == &prop) {
						return;
					}

					moProp::Copy(prop);

					const moPropSimple<T, TYPE>& simple = dynamic_cast<const moPropSimple<T, TYPE>&>(prop);
					f_value = simple.f_value;
				}

	virtual moPropSPtr	Duplicate(void) const
				{
					moPropSPtr simple = new moPropSimple<T, TYPE>(GetName());
					simple->Copy(*this);
					return simple;
				}

	virtual prop_type_t	GetType(void) const
				{
					return TYPE;
				}

	T			Get(void) const
				{
					return f_value;
				}

	void			Set(const T& value)
				{
					f_value = value;
					Signal();
				}

private:
	T			f_value;
};

typedef	moPropSimple<zint32_t,    moProp::MO_PROP_TYPE_INT>		moPropInt;
typedef moPropSimple<zint64_t,    moProp::MO_PROP_TYPE_LONG_LONG>	moPropLongLong;
typedef moPropSimple<zfloat_t,    moProp::MO_PROP_TYPE_FLOAT>		moPropFloat;
typedef moPropSimple<zdouble_t,   moProp::MO_PROP_TYPE_DOUBLE>		moPropDouble;

typedef	moSmartPtr<moPropInt>		moPropIntSPtr;
typedef	moSmartPtr<moPropLongLong>	moPropLongLongSPtr;
typedef	moSmartPtr<moPropFloat>		moPropFloatSPtr;
typedef	moSmartPtr<moPropDouble>	moPropDoubleSPtr;



// The object properties are used for objects which require an
// moSmartPtr to ensure proper reference counting at all time
template<class T, moProp::prop_type_t TYPE>
class MO_DLL_EXPORT moPropObject : public moProp
{
public:
				moPropObject(mo_name_t name)
					: moProp(name)
				{
				}
	virtual			~moPropObject()
				{
				}

	const char *		moGetClassName(void) const
				{
					// unfortunatly T and TYPE cannot be converted to a string in that message
					return "molib::moBase::moProp::moPropObject<class T, moProp::prop_type_t TYPE>";
				}

	virtual void		Copy(const moProp& prop)
				{
					if(this == &prop) {
						return;
					}

					moProp::Copy(prop);

					const moPropObject<T, TYPE>& object = dynamic_cast<const moPropObject<T, TYPE>&>(prop);
					f_value = object.f_value;
				}

	virtual moPropSPtr	Duplicate(void) const
				{
					moPropSPtr object = new moPropObject<T, TYPE>(GetName());
					object->Copy(*this);
					return object;
				}

	virtual prop_type_t	GetType(void) const
				{
					return TYPE;
				}

	// note that we are returning a reference to the value and
	// not a pointer to it; actually, since this value is a variable
	// member, changing it's reference count would not be so smart,
	// would it?
	const T&		Get(void) const
				{
					return f_value;
				}

	void			Set(const T& value)
				{
					f_value = value;
					Signal();
				}

private:
	T			f_value;
};

typedef moPropObject<moBaseSPtr, moProp::MO_PROP_TYPE_POINTER>		moPropPointer;
typedef moPropObject<moWCString, moProp::MO_PROP_TYPE_STRING>		moPropString;
typedef moPropObject<moBuffer, moProp::MO_PROP_TYPE_BINARY>		moPropBinary;





// The array object can be used as a property of properties which are
// ordered by index (instead of by name -- name which can't be garanteed
// to be in the same order each time you run your process -- see the
// property bag for a name based sorted list of properties)
//
// The type of the items can be forced in which case trying to insert
// items of another type fails with a throw.
class MO_DLL_EXPORT moPropArray : public moProp
{
public:
				moPropArray(mo_name_t name, prop_type_t type = MO_PROP_TYPE_UNKNOWN);
	virtual			~moPropArray();

	const char *		moGetClassName(void) const;

	virtual prop_type_t	GetType(void) const { return MO_PROP_TYPE_ARRAY; }
	prop_type_t		GetElementsType(void) const { return f_type; }

	virtual void		Copy(const moProp& prop);
	virtual moPropSPtr	Duplicate(void) const;

	void			Empty(void);
	moPropSPtr		Get(int item_no) const;
	bool			Set(int item_no, const moProp *prop);
	bool			Delete(int item_no);

	unsigned long		CountIndexes(void) const;
	int			ItemNoAtIndex(int index) const;
	moPropSPtr		GetAtIndex(int index) const;
	void			SetAtIndex(int index, const moProp *prop);
	bool			DeleteAtIndex(int index);

private:
	class moItem : public moBase
	{
	public:
					moItem(int item_no, const moProp *prop);
		virtual			~moItem();

		const char *		moGetClassName(void) const;

		virtual compare_t	Compare(const moBase& object) const;
		moItem&			operator = (const moItem& item);
		int			GetItemNo(void) const;
		moPropSPtr		Get(void) const;

	private:
		const int		f_item_no;
		moPropSPtr		f_prop;
	};

	typedef moSmartPtr<moItem>			moItemSPtr;
	typedef moTmplList<moItem, moSortedList>	moSortedListOfItems;

	moSortedListOfItems		f_array;
	const moProp::prop_type_t	f_type;
};


class MO_DLL_EXPORT moPropBagRef;


// The user can only use property references
// There is a basic property reference from which all the others are
// derived; all the properties have a corresponding reference
//moProp&
class MO_DLL_EXPORT moPropRef
{
public:
				moPropRef(const moName& name);
				moPropRef(const moPropRef& prop_ref);
				moPropRef(const moName& name, const moPropRef& prop_ref);

				// the following should be protected but somehow
				// the moPropBagRef can't use it then?!
				moPropRef(const moPropBag *link, const moProp *prop);

	virtual			~moPropRef();

	mo_name_t		GetName(void) const { return f_name; }
	virtual moProp::prop_type_t GetType(void) const;
	bool			IsNull(void) const { return !static_cast<bool>(f_prop); }
	moPropSPtr		GetProperty(void) const { return f_prop; }

	void			ClearRef(void) { f_prop = 0; }
	bool			HasProp(void) const { return static_cast<bool>(f_prop); }
	bool			IsLinked(void) const { return !f_link; }
	void			Link(const moPropBagRef prop_bag);
	moPropBagSPtr		GetLink(void) const { return f_link; }

	// The following operators can be a problem in case we want
	// the references to act like integers in some cases
	//			operator bool (void) { return IsNull(); }
	//bool			operator ! (void) { return !f_prop; }

	// it seems to me that we don't actually need the cast operator here
	//			operator moProp& (void) const { return *const_cast<moProp *>(static_cast<const moProp *>(f_prop)); }
	//moPropRef&		operator = (const moProp& prop) { f_prop->CopyFrom(prop); return *this; }

protected:
	const mo_name_t		f_name;
	moPropBagSPtr		f_link;
	moPropSPtr		f_prop;

private:
	// forbidden because you can't copy f_name
	// derived types define the assignment operator
	// and actually copy the value between properties
	moPropRef&		operator = (const moPropRef& prop_ref) { return *this; }
};





//moPropBag&
class MO_DLL_EXPORT moPropBagRef : public moPropRef
{
public:
				moPropBagRef(const moName& name);
				moPropBagRef(const moPropRef& prop_ref);
				moPropBagRef(const moName& name, const moPropRef& prop_ref);

				moPropBagRef(const moPropBag& bag);

	virtual			~moPropBagRef();

	virtual moProp::prop_type_t GetType(void) const
				{
					return moProp::MO_PROP_TYPE_PROP_BAG;
				}

	void			NewProp(void);
	bool			IsEmpty(void) const;
	void			Empty(void);
	unsigned long		Count(void) const;
	void			Dump(unsigned int flags = moPropBag::DUMP_FLAG_RECURSIVE, const char *message = 0) const;

	moPropRef		Get(const moIndexOrName index_or_name) const;
	moPropRef		operator [] (const moIndexOrName& index_or_name) const;

	void			Set(const moIndexOrName& index_or_name, const moPropRef& prop);
	moPropRef		operator [] (const moIndexOrName& index_or_name);

	moPropBagRef&		operator = (const moPropBag& bag);
	moPropBagRef&		operator = (const moPropBagRef& bag);

	// Test whether there is a reference to a property bag
	// (this is NOT the link -- equivalent to Get() ==/!= 0)
				operator bool (void) const;
	bool			operator ! (void) const;

	// Add the specified bag overwriting or not the existing properties
	void			Add(const moPropBagRef& prop_bag, bool overwrite = true);
	void			Add(const moPropRef& prop, bool overwrite = true);

	// Overwrite (actually very much like Set())
	moPropBagRef&		operator += (const moPropBagRef& bag);
	moPropBagRef&		operator += (const moPropRef& prop);
	moPropBagRef		operator + (const moPropBagRef& bag);
	moPropBagRef		operator + (const moPropRef& prop);

	friend moPropBagRef	operator + (const moPropRef& prop1, const moPropBagRef& prop2)
				{
					moPropBagRef result(prop2.PropBag());
					result.Add(prop1);
					return result;
				}

	friend moPropBagRef	operator + (const moPropRef& prop1, const moPropRef& prop2)
				{
					moPropBagRef result("no-name");
					result.NewProp();
					result.Add(prop1);
					result.Add(prop2);
					return result;
				}


	// Insert if missing only; ignore otherwise
	moPropBagRef&		operator |= (const moPropBagRef& bag);
	moPropBagRef&		operator |= (const moPropRef& prop);
	moPropBagRef		operator | (const moPropBagRef& bag);
	moPropBagRef		operator | (const moPropRef& prop);

	// Remove properties
	void			Delete(const moIndexOrName& index_or_name);
	void			Delete(const moPropBagRef& bag);		// remove from this all props found in bag
	moPropBagRef&		operator -= (const moIndexOrName& index_or_name);
	moPropBagRef&		operator -= (const moPropBagRef& bag);	// remove from this all props found in bag
	moPropBagRef		operator - (const moIndexOrName& index_or_name);
	moPropBagRef		operator - (const moPropBagRef& bag);

	// Create the intersection
	moPropBagRef&		operator &= (const moPropBagRef& bag);	// keep only common properties
	moPropBagRef		operator & (const moPropBagRef& bag);

	// Compute the comparison between two bags
	//compare_t		CompareBags(const moBase& prop_bag) const;
	//bool			operator == (const moPropBagRef& prop_bag) const;
	//bool			operator != (const moPropBagRef& prop_bag) const;
	//bool			operator < (const moPropBagRef& prop_bag) const;
	//bool			operator <= (const moPropBagRef& prop_bag) const;
	//bool			operator > (const moPropBagRef& prop_bag) const;
	//bool			operator >= (const moPropBagRef& prop_bag) const;

private:
	moPropBag&		PropBag(void) const { return *const_cast<moPropBag *>(dynamic_cast<const moPropBag *>(static_cast<const moProp *>(f_prop))); }
};











//moPropSimple&
template<class T, class CAST, moProp::prop_type_t TYPE>
class MO_DLL_EXPORT moPropSimpleRef : public moPropRef
{
public:
	typedef T		value_type_t;

				moPropSimpleRef(const moName& name)
					: moPropRef(name)
				{
				}

				moPropSimpleRef(const moPropRef& prop_ref)
					: moPropRef(prop_ref)
				{
				}

				moPropSimpleRef(const moName& name, const moPropRef& prop_ref)
					: moPropRef(name, prop_ref)
				{
				}

				moPropSimpleRef(const moPropSimple<T, TYPE>& prop_simple)
					: moPropRef(0, &prop_simple)
				{
				}

	const char *		moGetClassName(void) const
				{
					// unfortunately, a template can't have a parameter which is a string...
					return "molib::moBase::moPropRef::moPropSimpleRef<class T, class CAST, moProp::prop_type_t TYPE>";
				}

	virtual moProp::prop_type_t GetType(void) const
				{
					return TYPE;
				}

	void			AddHandler(moBase& handler)
				{
					PropSimple().AddHandler(handler);
				}
	void			RemoveHandler(moBase& handler)
				{
					PropSimple().RemoveHandler(handler);
				}
	void			Signal(void)
				{
					PropSimple().Signal();
				}

	T			Get(void) const
				{
					return PropSimple().Get();
				}

	template<class C>
	void			Set(const C& value)
				{
					PropSimple().Set(static_cast<const T&>(value));
				}

				operator CAST (void) const
				{
					return static_cast<CAST>(Get());
				}

				operator CAST (void)
				{
					return static_cast<CAST>(Get());
				}

				operator bool (void) const { return Get() != 0; }
				operator bool (void) { return Get() != 0; }
	bool			operator ! (void) const { return !Get(); }
	T			operator ~ (void) const { return ~Get(); }
	T			operator + (void) const { return +Get(); }
	T			operator - (void) const { return -Get(); }

	moPropSimpleRef<T, CAST, TYPE>& operator ++ (void) { { moPropAutoLock lock(PropSimple()); Set(Get() + 1); } return *this; }
	T			operator ++ (int) { T r; { moPropAutoLock lock(PropSimple()); r = Get(); Set(r + 1); } return r; }
	moPropSimpleRef<T, CAST, TYPE>& operator -- (void) { { moPropAutoLock lock(PropSimple()); Set(Get() - 1); } return *this; }
	T			operator -- (int) { T r; { moPropAutoLock lock(PropSimple()); r = Get(); Set(r - 1); } return r; }

	moPropSimpleRef<T, CAST, TYPE>& operator = (const moPropRef& prop_ref)
				{
					Set(dynamic_cast<const moPropSimpleRef<T, CAST, TYPE>& >(prop_ref).Get());
					return *this;
				}

	moPropSimpleRef<T, CAST, TYPE>& operator = (const moPropSimpleRef<T, CAST, TYPE>& prop)
				{
					Set(prop.Get());
					return *this;
				}

#define	prop_simple_assignment(type) \
	moPropSimpleRef<T, CAST, TYPE>& operator = (const type value) { Set(value); return *this; }

				prop_simple_assignment(bool);
				prop_simple_assignment(char);
				prop_simple_assignment(signed char);
				prop_simple_assignment(unsigned char);
				prop_simple_assignment(int16_t);
				prop_simple_assignment(uint16_t);
				prop_simple_assignment(int32_t);
				prop_simple_assignment(uint32_t);
#if __WORDSIZE == 32
				prop_simple_assignment(long);
				prop_simple_assignment(unsigned long);
#endif
				prop_simple_assignment(int64_t);
				prop_simple_assignment(uint64_t);
				prop_simple_assignment(float);
				prop_simple_assignment(double);
				// At this time long double is not useful and it is not
				// implemented on all platforms.

#define	prop_simple_operator(ass, op) \
	moPropSimpleRef<T, CAST, TYPE>& operator ass (const T& value) { { moPropAutoLock lock(PropSimple()); Set(Get() op value); } return *this; } \
	moPropSimpleRef<T, CAST, TYPE>& operator ass (const moPropSimpleRef<T, CAST, TYPE>& n) { { moPropAutoLock lock(PropSimple()); Set(Get() op n.Get()); } return *this; }

				prop_simple_operator(*=, *)
				prop_simple_operator(/=, /)
				prop_simple_operator(%=, %)
				prop_simple_operator(+=, +)
				prop_simple_operator(-=, -)
				prop_simple_operator(<<=, <<)
				prop_simple_operator(>>=, >>)
				prop_simple_operator(&=, &)
				prop_simple_operator(|=, |)
				prop_simple_operator(^=, ^)

	void			NewProp(void)
				{
					f_prop = new moPropSimple<T, TYPE>(f_name);
//fprintf(stderr, "Prop name = 0x%08X (0x%08X)\n", (int)f_name, (int)f_prop->GetName());
					if(f_link) {
						// the new property is not automatically linked
						// so we need to link it now
						f_link->Set(f_name, f_prop);
					}
				}

private:
	moPropSimple<T, TYPE>&	PropSimple(void) const { return *const_cast<moPropSimple<T, TYPE> *>(dynamic_cast<const moPropSimple<T, TYPE> *>(static_cast<const moProp *>(f_prop))); }
};


typedef	moPropSimpleRef<zint32_t,    int32_t,   moProp::MO_PROP_TYPE_INT>	moPropIntRef;
typedef moPropSimpleRef<zint64_t,    int64_t,   moProp::MO_PROP_TYPE_LONG_LONG>	moPropLongLongRef;
typedef moPropSimpleRef<zfloat_t,    float,     moProp::MO_PROP_TYPE_FLOAT>	moPropFloatRef;
typedef moPropSimpleRef<zdouble_t,   double,    moProp::MO_PROP_TYPE_DOUBLE>	moPropDoubleRef;


//moPropObject&
template<class T, moProp::prop_type_t TYPE>
class MO_DLL_EXPORT moPropObjectRef : public moPropRef
{
public:
				moPropObjectRef(const moName& name)
					: moPropRef(name)
				{
				}

				moPropObjectRef(const moPropRef& prop_ref)
					: moPropRef(prop_ref)
				{
				}

				moPropObjectRef(const moName& name, const moPropRef& prop_ref)
					: moPropRef(name, prop_ref)
				{
				}

				moPropObjectRef(const moPropObject<T, TYPE>& prop_object)
					: moPropRef(0, &prop_object)
				{
				}

	virtual moProp::prop_type_t GetType(void) const
				{
					return TYPE;
				}

	void			AddHandler(moBase& handler)
				{
					PropObject().AddHandler(handler);
				}
	void			RemoveHandler(moBase& handler)
				{
					PropObject().RemoveHandler(handler);
				}
	void			Signal(void)
				{
					PropObject().Signal();
				}

	const T&		Get(void) const
				{
					return PropObject().Get();
				}

	template<class C>
	void			Set(const C& value)
				{
					PropObject().Set(static_cast<const T&>(value));
				}

				operator T (void) const
				{
					return PropObject().Get();
				}

				operator T (void)
				{
					return PropObject().Get();
				}

	moPropObjectRef<T, TYPE>& operator = (const moPropRef& prop_ref)
				{
					Set(dynamic_cast<const moPropObjectRef&>(prop_ref).Get());
					return *this;
				}

	moPropObjectRef<T, TYPE>& operator = (const T& value)
				{
					Set(value);
					return *this;
				}

	template<class C>
	moPropObjectRef<T, TYPE>& operator = (const C& value)
				{
					Set(value);
					return *this;
				}

	template<class C>
	moPropObjectRef<T, TYPE>& operator = (const moPropObject<C, TYPE>& prop)
				{
					Set(PropObject().Get());
					return *this;
				}

	void			NewProp(void)
				{
					f_prop = new moPropObject<T, TYPE>(f_name);
					if(f_link) {
						// the new property is not automatically linked
						// so we need to link it now
						f_link->Set(f_name, f_prop);
					}
				}

private:
	moPropObject<T, TYPE>&	PropObject(void) const { return *const_cast<moPropObject<T, TYPE> *>(dynamic_cast<const moPropObject<T, TYPE> *>(static_cast<moProp *>(f_prop))); }
};


typedef moPropObjectRef<moBaseSPtr, moProp::MO_PROP_TYPE_POINTER>	moPropPointerRef;
typedef moPropObjectRef<moWCString, moProp::MO_PROP_TYPE_STRING>	moPropStringRef;
typedef moPropObjectRef<moBuffer,   moProp::MO_PROP_TYPE_BINARY>	moPropBinaryRef;






class MO_DLL_EXPORT moPropArrayRef : public moPropRef
{
public:
				moPropArrayRef(const moName& name);
				moPropArrayRef(const moPropRef& prop_ref);
				moPropArrayRef(const moName& name, const moPropRef& prop_ref);
				moPropArrayRef(const moPropArray& prop_array);

	moPropArrayRef&		operator = (const moPropArrayRef& array_ref);
	moPropArrayRef&		operator = (const moPropArray& array);

	virtual moProp::prop_type_t GetType(void) const;
	moProp::prop_type_t	GetElementsType(void) const;

	void			Empty(void);
	bool			Delete(int item_no);
	moPropSPtr		Get(int item_no) const;
	bool			Set(int item_no, const moProp *prop);
	bool			Set(int item_no, const moPropRef& prop);
	moPropSPtr		operator [] (int item_no) const;

	int			CountIndexes(void) const;
	int			ItemNoAtIndex(int index) const;
	bool			DeleteAtIndex(int index);
	moPropSPtr		GetAtIndex(int index) const;
	void			SetAtIndex(int index, const moProp *prop);

	void			NewProp(moProp::prop_type_t type = moProp::MO_PROP_TYPE_UNKNOWN);

private:
	moPropArray&		PropArray(void) const;
};



#if 0
typedef moPropArrayRef<moProp,              moPropRef,		moProp::MO_PROP_TYPE_ARRAY>			moPropArrayAnyRef;	// mixed types
typedef moPropArrayRef<moPropBag,           moPropBagRef,	moProp::MO_PROP_TYPE_ARRAY_PROP_BAG>		moPropArrayPropBagRef;
typedef moPropArrayRef<moPropInt,           moPropIntRef,	moProp::MO_PROP_TYPE_ARRAY_INT>			moPropArrayIntRef;
typedef moPropArrayRef<moPropLongLong,      moPropLongLongRef,	moProp::MO_PROP_TYPE_ARRAY_LONG_LONG>		moPropArrayLongLongRef;
typedef moPropArrayRef<moPropFloat,         moPropFloatRef,	moProp::MO_PROP_TYPE_ARRAY_FLOAT>		moPropArrayFloatRef;
typedef moPropArrayRef<moPropDouble,        moPropDoubleRef,	moProp::MO_PROP_TYPE_ARRAY_DOUBLE>		moPropArrayDoubleRef;
typedef moPropArrayRef<moPropPointer,       moPropPointerRef,	moProp::MO_PROP_TYPE_ARRAY_POINTER>		moPropArrayPointerRef;
typedef moPropArrayRef<moPropString,        moPropStringRef,	moProp::MO_PROP_TYPE_ARRAY_STRING>		moPropArrayStringRef;
typedef moPropArrayRef<moPropBinary,        moPropBinaryRef,	moProp::MO_PROP_TYPE_ARRAY_BINARY>		moPropArrayBinaryRef;
typedef moPropArrayRef<moPropArray<moProp, moProp::MO_PROP_TYPE_ARRAY>, moPropArrayRef<moProp, moPropRef, moProp::MO_PROP_TYPE_ARRAY>, moProp::MO_PROP_TYPE_ARRAY_ARRAY> moPropArrayArrayRef;	// an array of arrays, yes I know...
#endif




// class to be derived to load & save props to streams
class MO_DLL_EXPORT moPropIO : public moBase
{
public:
	virtual			~moPropIO();

	void			SetInput(moIStream *input_stream);
	void			SetOutput(moOStream *output_stream);
	void			SetSavePointers(bool save_pointers = false);

	int			Load(moPropBagRef& prop_bag);
	int			Save(const moPropBagRef& prop_bag);

	int			GetLastError(bool clear = true) const;

	virtual const char *	moGetClassName(void) const;

protected:
	virtual int		InternalLoad(moPropBagRef& prop_bag) = 0;
	virtual int		InternalSave(const moPropBagRef& prop_bag) = 0;
	void			SetError(int err);

	moIStreamSPtr		f_input;
	moOStreamSPtr		f_output;
	zbool_t			f_save_pointers;

private:
	mutable int		f_errno;
};

typedef moSmartPtr<moPropIO>	moPropIOSPtr;





};			// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_PROPS_H

