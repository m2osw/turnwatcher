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



#ifndef MO_AUTO_RESTORE_H
#define	MO_AUTO_RESTORE_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif


namespace molib
{

/** \brief Template used to auto-restore variables.
 *
 * This template is used whenever a variable needs to be restore
 * before the end of a function even if one of the sub-functions
 * being called generates an exception.
 *
 * This is, in effect, an RAII class that work with any type
 * T that can be copied (T a; T b; a = b;). Classes that do not
 * allow their assignment operator will not work with the
 * moAutoRestore functionality.
 *
 * Say you have a flag that you want to set to true while running
 * in a function, and restore to false before quitting, in all
 * cases because otherwise your object could not function anymore.
 * This is done with the following:
 *
 * \code
 * myClass::foo()
 * {
 *	// assuming that myClass as a zbool_t f_running variable member
 *	...
 *	// set f_running to true, make sure to restore to false on
 *	// exit of this function.
 *	moAutoRestore<zbool_t> auto_restore(f_running, true, false);
 *	...
 *	if(some condition) {
 *		// stopping early because of some error condition
 *		// f_running is set to false by the compiler!
 *		return;
 *	}
 *	...
 *	// here f_running is set to false by the compiler
 * }
 *
 * myClass::expr_mul()
 * {
 *	// assuming that myClass as a zint_t f_state variable member
 *	...
 *	// set f_state to the multiplication state, make sure to restore
 *	// to whatever the variable was set to before
 *	moAutoRestore<zint_t> auto_restore(f_state, MUL_STATE);
 *	...
 *	if(some condition) {
 *		// stopping early because of some error condition
 *		// f_state is restored by the compiler!
 *		return;
 *	}
 *	...
 *	// here f_state is restored by the compiler
 * }
 * \endcode
 *
 * \bug
 * Note that calling the Restore() function marks the object as
 * restored. In other words, the destructor has then no more
 * effect. This means you can restore your variable. Then set
 * it to another value and return from your scope and the variable
 * will not be \em restored again.
 *
 * \bug
 * Note that calling Set() or Reset() cancels the effect of Restore().
 * Thus, you want to change your variable and still be sure that
 * the destructor restores the value, use the Set() or Reset()
 * functions instead of other direct access operations and functions
 * on the saved object.
 */
template<typename T>
class MO_DLL_EXPORT moAutoRestore
{
public:
	/// The type of this moAutoRestore object.
	typedef T auto_restore_t;

	/** \brief Initializes an auto-restore variable.
	 *
	 * This contructor takes in a variable reference.
	 *
	 * The current variable value is saved so it can be restore
	 * whenever the moAutoRestore is destroyed or sooner whenever
	 * the Restore() function is called.
	 *
	 * You can then modify the variable as necessary. On destruction
	 * of this moAutoRestore, it will automatically be restored.
	 *
	 * When using this constructor, the Reset() and Restore()
	 * function behave the same way.
	 *
	 * \param[in] variable The variable to set or restore
	 */
	moAutoRestore<T>(T& variable)
		: f_variable(variable),
		  f_set(variable),
		  f_save(variable)
	{
	}

	/** \brief Initializes an auto-restore variable.
	 *
	 * This contructor takes in a variable reference and a value used
	 * to set the variable right away.
	 *
	 * The current variable value is saved so it can be restore
	 * whenever the moAutoRestore is destroyed or sooner whenever
	 * the Restore() function is called.
	 *
	 * In this case, the Reset() function resets the variable to
	 * the \p set parameter; and the Restore() function restores
	 * the variable to value it was set to at the time this
	 * constructor is called.
	 *
	 * \param[in] variable The variable to set or restore
	 * \param[in] set The value to set the variable on entry
	 */
	moAutoRestore<T>(T& variable, const T& set)
		: f_variable(variable),
		  f_set(set),
		  f_save(variable)
	{
		f_variable = set;
	}

	/** \brief Set and reset a variable to specific values.
	 *
	 * This constructor takes a variable reference, a value used
	 * to set the variable on entry and a value to reset the variable
	 * whenever the object is destroyed or the Reset() function is
	 * called.
	 *
	 * In this case, the Reset() function resets the variable to
	 * the \p set parameter; and the Restore() function restores
	 * the variable to the \p restore parameter.
	 *
	 * \param[in] variable The variable to attach to
	 * \param[in] set The value to set the variable at on entry
	 * \param[in] restore The value to reset the variable on destruction
	 */
	moAutoRestore<T>(T& variable, const T& set, const T& restore)
		: f_variable(variable),
		  f_set(set),
		  f_save(restore)
	{
		f_variable = set;
	}

	/** \brief Reset the variable
	 *
	 * This function resets the variable to the value specified as
	 * \p reset in the constructor.
	 *
	 * \bug
	 * If you called the Restore() function, the destructor will have
	 * no effect unless you called Set() or Reset() after the call to
	 * the Restore() function. This means that changing your variable
	 * value after a call to Restore() will prevent the destructor
	 * from restoring again.
	 */
	~moAutoRestore()
	{
		if(!f_restored) {
			Restore();
		}
	}

	/** \brief Set the variable to a new value.
	 *
	 * This function is the same as using the assignment operator on
	 * the variable. It is suggested that you do so instead. This
	 * Set() function does not change what the Reset() function will
	 * reset the variable to if called.
	 *
	 * \code
	 * foo()
	 * {
	 *	int var;	// in general this is used with variable members
	 *	moAutoRestore<int> safe_var(var, 0);
	 *
	 *	// the next two statements are equivalent
	 *	var = 5;
	 *	safe_var.Set(5);
	 *
	 *	...
	 * }		// here var is set to 0
	 * \endcode
	 *
	 * \bug
	 * If you use the Set() function, it resets the f_restored flag
	 * which means the destructor will restore the value. Whereas,
	 * just setting the value (var = 5;) after a call to Restore()
	 * will prevent the destructor from resetting the value again.
	 *
	 * \param[in] set The new value for the variable
	 */
	void Set(const T& set)
	{
		f_variable = set;
		f_restored = false;
	}

	/** \brief Reset the variable.
	 *
	 * This function sets the variable to the value specified in
	 * the constructor as the \p set parameter.
	 *
	 * \note
	 * If you use the Reset() function, it resets the f_restored flag
	 * which means the destructor will restore the value.
	 */
	void Reset()
	{
		f_variable = f_set;
		f_restored = false;
	}

	/** \brief Restore the variable.
	 *
	 * This function sets the variable to the value specified in
	 * the constructor as the \p restore parameter. If no \p restore
	 * parameter was specified, then it is restore to the value of
	 * the variable whenever the constructor was called.
	 *
	 * By default, this function is called whenever the moAutoRestore is
	 * being destroyed. This means, it is exception safe.
	 */
	void Restore()
	{
		f_variable = f_save;
		f_restored = true;
	}

	/** \brief Mark the variable as modified.
	 *
	 * This function marks the variable as modified meaning that
	 * the destructor needs to call Restore() before returning.
	 *
	 * In effect, this call cancels the effect of the Restore()
	 * function without directly modifying the variable (but you
	 * may modify the variable by calling some functions or
	 * using operators on it outside of this class.)
	 */
	void Modified()
	{
		f_restored = false;
	}

	/** \brief Mark the variable as having the proper value.
	 *
	 * This function marks the auto-restore object so that it
	 * will not restore anything in its destructor. This is
	 * useful in functions where many errors may occur and
	 * the state before starting is very important.
	 */
	void Done()
	{
		f_restored = true;
	}

	/** \brief Retrieve a copy of the saved value by reference.
	 *
	 * This function can be used to read the saved value.
	 *
	 * \return The saved value. This is the 1st or 3rd parameter on the constructor.
	 */
	const T& SavedValue() const
	{
		return f_save;
	}

private:
	// at this time, avoid copy operators
	moAutoRestore<T>(const moAutoRestore<T>& src);
	moAutoRestore<T>& operator = (const moAutoRestore<T>& rhs);

	T&		f_variable;
	T		f_set;
	T		f_save;
	zbool_t		f_restored;
};






};			// namespace molib;

// vim: ts=8
#endif		// #ifndef MO_XML_H

