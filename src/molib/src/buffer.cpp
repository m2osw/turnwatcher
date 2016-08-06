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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_buffer.h"
#endif

#include	"mo/mo_buffer.h"



namespace molib
{




/** \brief Initialize a buffer object
 *
 * The constructor of the buffer object can be used to also
 * set the buffer to the given data buffer of the specified
 * size. By default an empty buffer is created.
 * 
 * When data is a null pointer, then the size and max parameters are
 * ignored.
 * 
 * IMPORTANT: the data pointer given to the buffer must have
 * been allocated with the new[] operator and it will be
 * managed by the buffer object thus deleting this buffer
 * object will also delete the user data pointer.
 * 
 * Note that the max value should be at least equal to size.
 * When max is smaller than size, it is set to size. It can
 * be used to avoid some reallocations.
 *
 * \param[in] data 	A user data pointer used as the data buffer.
 * \param[in] size 	The size of the valid data in the buffer.
 * \param[in] max 	The exact size of the data buffer.
 *
 * \sa Set
 */
moBuffer::moBuffer(void *data, unsigned long size, unsigned long max)
{
	if(data == 0) {
		f_max = f_size = 0;
		f_data = 0;
	}
	else {
		f_max = max < size ? size : max;
		f_size = size;
		f_data = reinterpret_cast<char *>(data);
	}
}


/** \brief Copy constructor
 *
 * The copy operator will duplicate the source
 * buffer data in a newly allocated memory buffer.
 *
 * \param[in] buffer 	Another buffer to copy in this buffer.
 *
 * \sa Set
 */
moBuffer::moBuffer(const moBuffer& buffer)
	: moBase(buffer)
{
	f_max = f_size = 0;
	f_data = 0;
	Copy(0, buffer.f_data, buffer.f_size);
}


/** \brief Destroy a buffer object
 *
 * Destroys the encapsulated buffer properly.
 */
moBuffer::~moBuffer()
{
	delete [] f_data;
}



/** \brief Assignment operator
 *
 * The assignment operator will duplicate the source
 * buffer data in a newly allocated memory buffer.
 *
 * \param[in] buffer 	Another buffer to copy in this buffer.
 *
 * \return The assignment operator returns a reference to this.
 *
 * \sa Set
 */
moBuffer& moBuffer::operator = (const moBuffer& buffer)
{
	SetSize(buffer.f_size);
	Copy(0, buffer.f_data, buffer.f_size);
	return *this;
}


/** \brief Initialize a buffer object
 *
 * The Set() function deletes any previously defined data buffer and
 * replaces it with the new information.
 * 
 * When data is a null pointer, then the size and max parameters are
 * ignored.
 * 
 * IMPORTANT: the data pointer given to the buffer must have
 * been allocated with the new[] operator and it will be
 * managed by the buffer object thus deleting this buffer
 * object will also delete the user data pointer.
 * 
 * Note that the max value should be at least equal to size.
 * When max is smaller than size, it is set to size. It can
 * be used to avoid some reallocations.
 *
 * \param[in] data 	A user data pointer used as the data buffer.
 * \param[in] size 	The size of the valid data in the buffer.
 * \param[in] max 	The exact size of the data buffer.
 *
 * \sa moBuffer::moBuffer
 */
void moBuffer::Set(void *data, unsigned long size, unsigned long max)
{
	delete [] f_data;
	if(data == 0) {
		f_max = f_size = 0;
		f_data = 0;
	}
	else {
		f_max = max < size ? size : max;
		f_size = size;
		f_data = reinterpret_cast<char *>(data);
	}
}


/** \brief Initialize a buffer object
 *
 * The copy operator will duplicate the source
 * buffer data in a newly allocated memory buffer.
 *
 * \param[in] buffer 	Another buffer to copy in this buffer.
 *
 * \sa Set, moBuffer::moBuffer
 */
void moBuffer::Set(const moBuffer& buffer)
{
	Set(buffer.f_data, buffer.f_size, buffer.f_max);
}


/** \brief Change the size of the buffer
 *
 * The SetSize() function enlarges the buffer and optionally
 * clears the new data bytes. Note that the data pointer
 * may change in this operation.
 * 
 * The buffer can also be shrunk in which case the clear
 * flag is unused.
 * 
 * Setting the size to zero will clear the buffer (make it
 * empty).
 *
 * \param[in] size	The new buffer size.
 * \param[in] clear	Whether to clear the extra bytes.
 *
 * \sa Copy, Append, Insert, Fill
 */
void moBuffer::SetSize( unsigned long size, bool clear )
{
	char		*data;

	if(size <= f_size) {
		f_size = size;
		if(size == 0) {
			f_max = 0;
			delete [] f_data;
			f_data = 0;
		}
	}
	else if(size <= f_max) {
		if(clear) {
			memset(f_data + f_size, 0, size - f_size);
		}
		f_size = size;
	}
	else {
		// TODO: should we not decide to make the buffer
		//	 larger right away (f_max = size + <step>)
		data = new char[size];
		memcpy(data, f_data, f_size);	/* Flawfinder: ignore */
		delete [] f_data;
		f_data = data;
		f_size = size;
		f_max = size;
	}
}


/** \brief Clears the data buffer.
 *
 * The Empty() function is equivalent to setting the buffer
 * to a null data pointer (Set()) or setting the size to
 * zero (SetSize(0)).
 * 
 * \sa Set, SetSize
 */
void moBuffer::Empty(void)
{
	Set();
}


/** \brief Returns true when the buffer is empty.
 *
 * The IsEmpty() function returns true whenever the size
 * of the buffer is zero.
 *
 * \sa Set, SetSize
 */
bool moBuffer::IsEmpty(void) const
{
	return f_size == 0;
}


/** \brief Copy the specified data in the buffer.
 *
 * The Copy() function can be used to change the data in a
 * buffer. It will automatically enlarge the buffer if required.
 * (i.e. if you try to copy past the end of the current buffer.)
 *
 * This function will ensure that the buffer is large
 * enough and automatically enlarge the buffer if necessary.
 * This means the data pointer can move in memory.
 *
 * \param[in] pos	Where in the buffer the data is copied.
 * \param[in] data	The user data to be copied.
 * \param[in] size	The number of bytes to copy of data, if zero, nothing happens.
 *
 * \sa SetSize
 */
void moBuffer::Copy(unsigned long pos, const void *data, unsigned long size)
{
	if(size > 0) {
		// ensure the buffer is large enough
		if(pos == (unsigned long) -1) {
			pos = f_size;
		}
		if(pos + size > f_size) {
			SetSize(pos + size);
		}

		memcpy(f_data + pos, data, size);	/* Flawfinder: ignore */
	}
}


/** \brief Append the specified data to the buffer.
 *
 * The Append() function can be used to enlarge the buffer and
 * copy the specified data at the end.
 *
 * This function will ensure that the buffer is large
 * enough and automatically enlarge the buffer if necessary.
 * This means the data pointer can move in memory.
 *
 * \param[in] data	The user data to be copied.
 * \param[in] size	The number of bytes to copy of data, if zero, nothing happens.
 *
 * \sa SetSize
 */
void moBuffer::Append(const void *data, unsigned long size)
{
	Copy((unsigned long) -1, data, size);
}


/** \brief Insert the specified data in the buffer.
 *
 * The Insert() function can be used to enlarge the buffer and
 * copy the specified data at the specified position. Data which
 * were at that position before is first moved after the new data.
 *
 * This function will ensure that the buffer is large
 * enough and automatically enlarge the buffer if necessary.
 * This means the data pointer can move in memory.
 *
 * \param[in] pos	Where in the buffer the data is copied.
 * \param[in] data	The user data to be copied.
 * \param[in] size	The number of bytes to copy of data, if zero, nothing happens.
 *
 * \sa SetSize
 */
void moBuffer::Insert(unsigned long pos, const void *data, unsigned long size)
{
	if(size > 0) {
		SetSize(f_size + size);

		memmove(f_data + pos + size, f_data + pos, f_size - pos);
		memcpy(f_data + pos, data, size);	/* Flawfinder: ignore */
	}
}


/** \brief Fill repeating the specified data in the buffer.
 *
 * The Fill() function can be used to fill a buffer with some
 * arbitrary pattern. The pattern is copied over length bytes
 * in the buffer at the specified position. The pattern is
 * an array of size bytes. If size or length is zero, nothing
 * happen.
 * 
 * This function will ensure that the buffer is large
 * enough and automatically enlarge the buffer if necessary.
 * This means the data pointer can move in memory.
 *
 * \param[in] pos	Where in the buffer the data is copied.
 * \param[in] data	The user data to be copied.
 * \param[in] size	The number of bytes to copy of data, if zero, nothing happens.
 *
 * \sa SetSize
 */
void moBuffer::Fill(unsigned long pos, unsigned long length, const void *data, unsigned long size)
{
	if(size > 0 && length > 0) {
		SetSize(pos + length);

		for(;;) {
			if(size >= length) {
				memcpy(f_data + pos, data, length);	/* Flawfinder: ignore */
				break;
			}
			memcpy(f_data + pos, data, size);	/* Flawfinder: ignore */
			pos += size;
			length -= size;
		}
	}
}


/** \brief Get the data pointer and its size.
 *
 * The Get() function will be used to retrieve a pointer of
 * the data in the buffer and the size of the buffer at once.
 *
 * \note
 * The data pointer returned by this function will certainly
 * change if you are using one of the Set(), SetSize(), Copy(),
 * Append(), Insert() or Fill() functions.
 *
 * \param data Copy the buffer pointer here.
 * \param size Copy the buffer size here.
 *
 * \sa Set, SetSize, Copy, Append, Insert, Fill
 */
void moBuffer::Get(void *& data, unsigned long& size) const
{
	data = reinterpret_cast<void *>(const_cast<char *>(f_data));
	size = f_size;
}


/** \brief Cast operator to retrieve the pointer.
 *
 * The void * cast operator can be used to get the data pointer
 * out of the buffer:
 * 
 * \code
 * 	moBuffer	buffer;
 * 	ptr = (<ptr type>) (void *) buffer;
 * \endcode
 *
 * \param data Copy the buffer pointer here.
 * \param size Copy the buffer size here.
 *
 * \sa Get(), Set(), SetSize(), Copy(), Append(), Insert(), Fill()
 *
 * \note The data pointer returned by this function will certainly
 * change if you are using one of the Set(), SetSize(), Copy(),
 * Append(), Insert() or Fill() functions.
 */
moBuffer::operator void * (void) const
{
	return reinterpret_cast<void *>(const_cast<char *>(f_data));
}


/** \brief Cast operator to retrieve the const pointer.
 *
 * The void * cast operator can be used to get the data pointer
 * out of the buffer:
 * 
 * \code
 *	moBuffer	buffer;
 *	ptr = (<ptr type>) (void *) buffer;
 * \endcode
 *
 * \note
 * The data pointer returned by this function will certainly
 * change if you are using one of the Set(), SetSize(), Copy(),
 * Append(), Insert() or Fill() functions.
 *
 * \param data Copy the buffer pointer here.
 * \param size Copy the buffer size here.
 *
 * \sa Get(), Set(), SetSize(), Copy(), Append(), Insert(), Fill()
 */
moBuffer::operator const void * (void) const
{
	return reinterpret_cast<const void *>(f_data);
}


/** \brief Get the size of the buffer.
 *
 * This function behaves the same as the overloaded integer
 * operators.
 *
 * \note
 * The data pointer returned by this function will certainly
 * change if you are using one of the Set(), SetSize(), Copy(),
 * Append(), Insert() or Fill() functions.
 *
 * \sa Get(), Set(), SetSize(), Copy(), Append(), Insert(), Fill()
 */
unsigned long moBuffer::GetSize(void) const
{
	return f_size;
}


/** \brief Retrieve the size of the buffer.
 *
 * This cast operator returns the size of the buffer in bytes.
 *
 * \sa GetSize
 */
moBuffer::operator unsigned int () const
{
	return f_size;
}


/** \brief Retrieve the size of the buffer.
 *
 * This cast operator returns the size of the buffer in bytes.
 *
 * \sa GetSize
 */
moBuffer::operator signed int () const
{
	return f_size;
}


/** \brief Retrieve the size of the buffer.
 *
 * This cast operator returns the size of the buffer in bytes.
 *
 * \sa GetSize
 */
moBuffer::operator unsigned long () const
{
	return f_size;
}


/** \brief Retrieve the size of the buffer.
 *
 * This cast operator returns the size of the buffer in bytes.
 * \sa GetSize
 */
moBuffer::operator signed long (void) const
{
	return f_size;
}


};			// namespace molib;

// vim: ts=8 sw=8 syntax=cpp.doxygen

