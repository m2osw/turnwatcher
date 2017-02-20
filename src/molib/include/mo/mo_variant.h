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



#ifndef MO_VARIANT_H
#define	MO_VARIANT_H

#include <mo/mo_base.h>

namespace molib
{

class MO_DLL_EXPORT moVariantBase
{
};

class MO_DLL_EXPORT moVariant
{
	public:
		moVariant();
		~moVariant();

		typedef bool				Boolean;
		typedef int32_t				Integer;
		typedef uint32_t			UInteger;
		typedef double				Real;
		typedef molib::moWCString	String;
		//
		// TODO: add some sort of blob support

		// Constructors
		//
		moVariant( const Boolean	val )			{ Assign(val); }
		moVariant( const Integer	val )			{ Assign(val); }
		moVariant( const UInteger	val )			{ Assign(val); }
		moVariant( const Real		val )			{ Assign(val); }
		moVariant( const String&	val )			{ Assign(val); }
		moVariant( const moVariant& obj )			{ Assign(val); }

		// Default assignment
		//
		moVariant& operator=( const Boolean		val )	{ Assign(val); return *this; }
		moVariant& operator=( const Integer		val )	{ Assign(val); return *this; }
		moVariant& operator=( const UInteger	val )	{ Assign(val); return *this; }
		moVariant& operator=( const Real		val )	{ Assign(val); return *this; }
		moVariant& operator=( const String&		val )	{ Assign(val); return *this; }
		moVariant& operator=( const moVariant&	val )	{ Assign(val); return *this; }

		void Assign( const Boolean		val );
		void Assign( const Integer		val );
		void Assign( const UInteger 	val );
		void Assign( const Real			val );
		void Assign( const String&		val );
		void Assign( const moVariant&	val );

		Boolean		asBoolean()		const;
		Integer		asInteger()		const;
		UInteger	asUInteger()	const;
		Real		asReal()		const;
		String&		asString()		const;
		//
		operator Boolean	() const	{ return asBoolean();	}
		operator Integer	() const	{ return asInteger();	}
		operator UInteger	() const	{ return asUInteger();	}
		operator Real		() const	{ return asReal();		}
		operator String&	() const	{ return asString();	}

		// Some platforms have a with automatically converting boolean negation into boolean
		//
		bool operator!		() const	{ return !asBoolean();	}


		typedef enum
		{
			TypeUndefined
				, TypeBoolean
				, TypeInteger
				, TypeUInteger
				, TypeReal
				, TypeString
		}
		Type;

		Type	GetType() const	{ return f_type; }

		// Automatic cast protection
		// These prevent casting that the programmer most likely did not intend...
		//
		moVariant( const void * );
		void Assign( const void * );
		moVariant& operator =( const void* );

	private:
		Type	f_type;
};

}
//namespace molib

#endif	// MO_VARIANT_H

// vim: ts=4 sw=4

