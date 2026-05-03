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
#pragma implementation "mo/mo_crypt.h"
#endif

#include	"mo/mo_crypt.h"

namespace molib
{


/*! \brief Initializes an encryption object (encoder).
 *
 * This function initializes a list of encryption objects.
 * 
 * Once such a list is initialized one can add moCrypt&gt;name&lt;
 * objects to it. These will be used to Encode() and/or
 * Decode() buffers of data.
 *
 * \sa moCrypt::~moCrypt, Encode, Decode
 */
moCrypt::moCrypt(void)
{
	f_secure = false;
}


/*! \brief Makes the Encode/Decode more secure.
 *
 * This function turns a flag ON so the Encode() and Decode() functions
 * will clear all the buffers used before to delete them.
 *
 * \sa Encode, Decode
 */
void moCrypt::Secure(void)
{
	f_secure = true;
}



/*! \brief Run the encoding process.
 *
 * This function will be used to run an encoding of a list of crypt objects.
 * The functions work by calling the encryption (Encode) function of each
 * moCryptBase object which was inserted in the moCrypt list.
 * 
 * \param[in]  src	Source buffer.
 * \param[in]  src_size Size of the source buffer.
 * \param[out] dst	A pointer to void set to point to the result.
 * \param[out] dst_size The size of the destination buffer.
 * 
 * \note The Encode() function is run from the first (index = 0) to
 * the last (index = Count() - 1) before returning the result.
 *
 * \sa Decode
 *
 */
void moCrypt::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) const
{
	long		idx, max;
	moCryptBase	*c;

	max = Count();
	idx = 0;
	while(idx < max) {
		c = dynamic_cast<moCryptBase *>(Get(idx));
		c->Encode(src, src_size, dst, dst_size);
		if(idx != 0) {
			if(f_secure) {
				memset(const_cast<unsigned char *>(src), 0, src_size);
			}
			delete src;
		}
		if(dst == 0) {
			// something failed, exit right away
			break;
		}
		// for next loop, ensure the source is the result of the last call
		src = dst;
		src_size = dst_size;
		idx++;
	}
}


/*! \brief Run the encoding process.
 *
 * This function will be used to run a decoding of a list of crypt objects.
 * The functions work by calling the decryption (Decode) function of each
 * moCryptBase object which was inserted in the moCrypt list.
 * 
 * \param[in]  src	Source buffer.
 * \param[in]  src_size Size of the source buffer.
 * \param[out] dst	A pointer to void set to point to the result.
 * \param[out] dst_size The size of the destination buffer.
 * 
 * \note The Decode() function is run from the last (index = Count() - 1)
 * to the first (index = 0) before returning the result.
 *
 * \sa Encode
 *
 */
void moCrypt::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) const
{
	long		idx, max;
	moCryptBase	*c;

	max = Count();
	idx = max;
	max--;
	while(idx > 0) {
		idx--;
		c = dynamic_cast<moCryptBase *>(Get(idx));
		c->Decode(src, src_size, dst, dst_size);
		if(idx != max) {
			if(f_secure) {
				memset(const_cast<unsigned char *>(src), 0, src_size);
			}
			delete src;
		}
		if(dst == 0) {
			// something failed, exit right away
			break;
		}
		// for next loop, ensure the source is the result of the last call
		src = dst;
		src_size = dst_size;
	}
}


/*! \brief Determines the minimum size necessary for decoding a buffer.
 *
 * This function determines the size necessary to decode an input buffer.
 * It will actually call the DecodeSize() function of the last moCryptBase
 * object part of this crypt list. If the size needs special handling,
 * then a special class can be created with a DecodeSize() returning the
 * proper size.
 * 
 * \param[in]  src	Source buffer.
 * \param[in]  src_size Size of the source buffer.
 * 
 * \note The Decode() function is run from the last (index = Count() - 1)
 * to the first (index = 0) before returning the result.
 *
 * \bug When there are no encoder/decoder in a list, this function returns 0.
 *
 * \sa Decode
 *
 */
long moCrypt::DecodeSize(const unsigned char *src, unsigned long src_size) const
{
	unsigned long	idx;

	idx = Count();
	if(idx > 0) {
		return dynamic_cast<moCryptBase *>(Get(idx - 1))->DecodeSize(src, src_size);
	}

	// no entry!!!
	return 0;
}



/*! \brief Call the Reset() function of each encoder/decoder.
 *
 * This function calls the Reset() function of each encoder and
 * decoder within this crypt object.
 * 
 * \sa Decode
 *
 */
void moCrypt::Reset(void) const
{
	unsigned long	idx;

	idx = Count();
	while(idx > 0) {
		idx--;
		dynamic_cast<moCryptBase *>(Get(idx))->Reset();
	}
}


/*! \brief Load tables previous saved with SaveTables.
 *
 * This function loads tables from the specified file in each of
 * the moCryptBase object.
 *
 * \param [in] input File opened for at least reading.
 *
 * \sa SaveTables
 */
void moCrypt::LoadTables(moFile& input)
{
	unsigned long	idx, max;

	max = Count();
	idx = 0;
	while(idx < max)
	{
#ifdef MO_DEBUG
	printf("Load table #%ld / %ld.\n", idx, max);
#endif
		dynamic_cast<moCryptBase *>(Get(idx))->LoadTable(input);
#ifdef MO_DEBUG
	printf("Loaded.\n");
#endif
		idx++;
	}
}



/*! \brief Save tables for later retrieval.
 *
 * This function saves tables to the specified file in order to
 * read them later and have/use the same tables.
 *
 * \param[in] output File opened for at least writing.
 *
 * \note The saving is specific to the endian of the computer being
 * used. Thus, if the tables are to be shared between computers
 * of different endians, there is a need to fix these functions.
 *
 * \sa LoadTables
 *
 */
void moCrypt::SaveTables(moFile& output) const
{
	unsigned long	idx, max;

	max = Count();
	idx = 0;
	while(idx < max) {
		dynamic_cast<moCryptBase *>(Get(idx))->SaveTable(output);
		idx++;
	}
}



/*! \brief Initializes an moCryptBase object.
 * 
 * This function initializes an moCryptBase object. This is a
 * virtual class which needs to be derived so there is an
 * Encode() and a Decode() set of functions.
 *
 * \sa Encode, Decode, moCryptBase::~moCryptBase
 *
 */
moCryptBase::moCryptBase(void)
{
	f_table = 0;
	f_table_size = 0;	// some objects may check this instead of the pointer...

	f_random = 0;
}




/*! \brief Cleans up a moCryptBase object.
 *
 * This function cleans up an moCryptBase object before
 * deallocation.
 *
 * \sa moCryptBase::moCryptBase
 *
 */
moCryptBase::~moCryptBase()
{
	delete f_table;
}




/*! \brief Create a table randomly for this object.
 *
 * This function can be used to generate a table for this crypt
 * object. This is a default way to generate tables for the crypt
 * object and don't need to be used. The user can instead create
 * its own tables with his own algorithms.
 *
 * \param [in] random	A pointer to a random object used to generate the * table.
 * \param [in] size 	Size of the table to generate.
 *
 * \note The size parameter can be set to zero in which case the table
 *	 note is removed.
 *
 * \sa RandomTable, SetTable, NewTable, CheckTableSize
 *
 */
void moCryptBase::GenerateTable(moRandomBase *random, unsigned long size)
{
	moRandomBase *save;

	// ask this crypt object to check the size parameter
	// (it has therefore a chance to adjust it - possibly setting it to zero)
	CheckTableSize(size);

	// reallocate the table only if necessary
	if(f_table_size != size) {
		delete f_table;
		if(size == 0) {
			f_table_size = 0;
			f_table = 0;
		}
		else {
			f_table = new unsigned char[size];
			if(f_table == 0) {
				f_table_size = 0;
				Reset();
				throw std::bad_alloc();
			}
			f_table_size = size;
		}
	}

	if(f_table_size != 0) {
		save = f_random;
		f_random = random;
		GenerateTable();
		f_random = save;
	}

	Reset();

	// special handling (debug function) of the higher level crypt object
	NewTable();
}


void moCryptBase::GenerateTable(void)
{
	// do nothing by default
}



/*! \brief Creates a fully random table for this object.
 *
 * This function generates a completely random set of data in the
 * current table using the current random object.
 * 
 * If no random object is defined or the table has a size of zero,
 * nothing happens.
 * 
 * Usually this function is called by the GenerateTable() of
 * classes derived from the moCryptBase when a random table is
 * sufficient and satisfactory.
 *
 * \sa GenerateTable, CheckTableSize
 *
 */
void moCryptBase::RandomTable(void)
{
	unsigned char	*s;
	unsigned long	i;

	if(f_table_size != 0 && f_random != 0) {
		s = f_table;
		i = f_table_size;
		while(i > 0) {
			i--;
			*s++ = static_cast<unsigned char>(f_random->Random() >> 24);
		}
	}
}



/*! \brief Check the size of a GenerateTable() call.
 *
 * This function is the default CheckTableSize() function. It
 * will do nothing.
 * 
 * Specific crypt objects can clear the size so no table is
 * generated. Some others may require a very specific size
 * and they can use the opportunity to make sure the size is
 * correct.
 *
 * \sa GenerateTable, RandomTable
 *
 */
void moCryptBase::CheckTableSize(unsigned long& size)
{
	// do nothing, we accept the size as it is by default
}



/*! \brief Set the encryption/decryption table.
 *
 * \param[in] table	A pointer to a user table.
 * \param[in] size	Size of the user table data.
 *
 * Changes the current table of the encryption object to
 * the one specified.
 * 
 * The SetTable() function makes a copy of the user table
 * within the moCryptBase object.
 * 
 * Note that the way the data will be used by the corresponding
 * encryption object is not specified in the moCryptBase object.
 * Thus, there is no enforcement at this level for a specific
 * size or type of data.
 *
 * \bug The input table pointer was given a type of unsigned char for
 * 	practicability. It is not intended to force any of the tables
 * 	to be of that type since some are clearly not.
 *
 * \sa SaveTable, LoadTable, Encode, Decode
 *
 */
void moCryptBase::SetTable(const unsigned char *table, unsigned long size)
{
	delete f_table;
	f_table_size = size;
	if(size == 0) {
		f_table = 0;
	}
	else {
		f_table = new unsigned char[size];
		if(f_table == 0) {
			f_table_size = 0;
			Reset();
			throw std::bad_alloc();
		}
		memcpy(f_table, table, size);
	}

	Reset();

	// special handling (debug function) of the higher level crypt object
	NewTable();
}



/*! \brief Called once a new table was installed.
 *
 * This function is the default used when no specific handling
 * is necessary whenever a new table is installed in a crypt
 * object.
 * 
 * Higher level crypt object may redefine this object in order
 * to check the table to ensure its validity or to generate
 * tables for faster encoding/decoding schemes.
 *
 * \sa SetTable
 *
 */
void moCryptBase::NewTable(void)
{
	// do nothing by default we accept the table as is
}








/*! \brief Save the current encode/decode table.
 *
 * This function save the current encode/decode table in such a
 * way that it can be reloaded by the LoadTable() function.
 *
 * \param[in] output An moFile created at least for writing.
 *
 *
 * \bug The tables are saved in the endian of the computer being
 * used at the time. If the tables are to be shared between
 * computers of different endians, then this function has to
 * be reworked or another function used.
 *
 * \sa LoadTable, SetTable
 *
 */
void moCryptBase::SaveTable(moFile& output)
{
	if(output.Write(&f_table_size, sizeof(f_table_size)) != (int) sizeof(f_table_size)) {
		throw moError(MO_ERROR_IO, "can't write an moCryptBase table size");
	}
	if(output.Write(f_table, f_table_size) != (int) f_table_size) {
		throw moError(MO_ERROR_IO, "can't write an moCryptBase table data");
	}
}



/*! \brief Load an encode/decode table.
 *
 * This function reads an encode/decode table which was
 * previously written by the corresponding SaveTable()
 * function.
 *
 * \param[in] input An moFile created at least for reading.
 *
 * \bug The tables are saved in the endian of the computer being
 * used at the time. If the tables are to be shared between
 * computers of different endians, then this function has to
 * be reworked or another function used.
 *
 * \sa SaveTable, SetTable
 *
 */
void moCryptBase::LoadTable(moFile& input)
{
#ifdef MO_DEBUG
printf("Read to delete table at %p\n", f_table);
#endif
	delete f_table;

#ifdef MO_DEBUG
printf("Read data...\n");
#endif

	if(input.Read(&f_table_size, sizeof(f_table_size)) != static_cast<int>(sizeof(f_table_size))) {

#ifdef MO_DEBUG
printf("Ooops read failed!\n");
#endif

		f_table = 0;
		f_table_size = 0;
		Reset();
		throw moError(MO_ERROR_IO, "can't read an moCryptBase table size");
	}

#ifdef MO_DEBUG
printf("Allocate table of %ld bytes\n", f_table_size);
#endif

	f_table = new unsigned char[f_table_size];
	if(f_table == 0) {
		if(f_table_size == 0) {
			Reset();
			return;
		}
		f_table_size = 0;
		Reset();
		throw std::bad_alloc();
	}
	if(input.Read(f_table, f_table_size) != static_cast<int>(f_table_size)) {
		delete f_table;
		f_table = 0;
		f_table_size = 0;
		Reset();
		throw moError(MO_ERROR_IO, "can't read an moCryptBase table data");
	}
	Reset();
}


/*! \brief Initializes an moCryptAdd8 object
 *
 * This function initializes an moCryptAdd8 object.
 *
 * \sa Encode, Decode, Destructor
 *
 */
moCryptAdd8::moCryptAdd8(void)
{
	Reset();
}


/*! \brief Initializes counter.
 *
 * This function resets an moCryptAdd8 indexes.
 *
 * \sa Encode, Decode, Destructor
 *
 */
void moCryptAdd8::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}


/*! \brief Generate a random table.
 *
 * This function calls the RandomTable() base function.
 *
 * \sa GenerateTable, RandomTable, CheckTableSize
 *
 */
void moCryptAdd8::GenerateTable(void)
{
	RandomTable();
}


/*! \brief Encodes the source buffer.
 *
 * This function encodes the source buffer by adding the values found
 * in the table at a similar offset. The operation looks like this:
 *
 * \code
 * 	dst[index] = src[index] + table[index % table_size];
 * \endcode
 * 
 * If there is no table, the index modulo 256 is added instead.
 * The source and destination values are taken as bytes.
 *
 * \param[in]  src	Source buffer.
 * \param[in]  src_size Size of the source buffer.
 * \param[out] dst 	A pointer to void set to point to the result.
 * \param[out] dst_size The size of the destination buffer.
 *
 * \sa Decode
 *
 */
void moCryptAdd8::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size) {
				f_encode_index = 0;
			}
		}
		dst[i] = src[i] + v;
		i++;
	}
}



/*! \brief Decodes the source buffer.
 *
 * This function removes the effect of a previous call to the Encode()
 * function of the same object with the same table.
 * 
 * It will actually substract the give values to each entry in the table.
 * The source and destination values are taken as bytes.
 *
 * \param[in]  src	Source buffer
 * \param[in]  src_size	Size of the source buffer
 * \param[out] dst	A pointer to void set to point to the result
 * \param[out] dst_size	The size of the destination buffer
 *
 * \sa Encode, DecodeSize
 *
 */
void moCryptAdd8::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size) {
				f_decode_index = 0;
			}
		}
		dst[i] = src[i] - v;
		i++;
	}
}


/*! \brief Minimum size to properly decode a source buffer.
 *
 * This function returns the minimum number of bytes necessary in order to
 * decode a buffer with this decoder.
 * 
 *
 * \param[in] src	Source buffer.
 * \param[in] src_size	Size of the source buffer.
 *
 * \sa Encode, Decode
 *
 */
long moCryptAdd8::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 1;
}






/*! \brief Initializes an moCryptAdd16 object.
 *
 * This function initializes an moCryptAdd16 object.
 *
 * \sa Encode, Decode, Destructor
 *
 */
moCryptAdd16::moCryptAdd16(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptAdd16 indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptAdd16::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}




/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	GenerateTable - generate a random table

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function calls the RandomTable() base function.

SEE ALSO

	GenerateTable, RandomTable, CheckTableSize

*/
void moCryptAdd16::GenerateTable(void)
{
	RandomTable();
}




/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	NewTable - check new table validity

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function ensures the new table is valid by checking the offsets
	against the size of the table.

SEE ALSO

	SetTable

*/
void moCryptAdd16::NewTable(void)
{
	if((f_table_size & 1) != 0) {
		throw moError(MO_ERROR_INVALID, "invalid table size - must be a multiple of %d bytes", 2);
	}
}





/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by adding the values found
	in the table at a similar offset. The operation looks like this:

		dst[index] = src[index] + table[index % table_size];

	If there is no table, the index modulo 256 is added instead.

	The source and destination values are taken as bytes.

SEE ALSO

	Decode

*/
void moCryptAdd16::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned short	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 2;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned short>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = reinterpret_cast<unsigned short *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size / 2) {
				f_encode_index = 0;
			}
		}
		reinterpret_cast<unsigned short *>(dst)[i] = reinterpret_cast<const unsigned short *>(src)[i] + v;
		i++;
	}

	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[dst_size - 1];
	}
}






/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually substract the give values to each entry in the table.

	The source and destination values are taken as bytes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptAdd16::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned short	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 2;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned short>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = reinterpret_cast<unsigned short *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size / 2) {
				f_decode_index = 0;
			}
		}
		reinterpret_cast<unsigned short *>(dst)[i] = reinterpret_cast<const unsigned short *>(src)[i] - v;
		i++;
	}

	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[dst_size - 1];
	}
}







/************************************************************ DOC:

CLASS

	moCryptAdd16

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptAdd16::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 2;
}






/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	Constructor - initializes an moCryptXOr object

SYNOPSIS

	moCryptXOr(void);

DESCRIPTION

	This function initializes an moCryptXOr object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptXOr::moCryptXOr(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptXOr indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptXOr::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}




/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	GenerateTable - generate a random table

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function calls the RandomTable() base function.

SEE ALSO

	GenerateTable, RandomTable, CheckTableSize

*/
void moCryptXOr::GenerateTable(void)
{
	RandomTable();
}





/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by adding the values found
	in the table at a similar offset. The operation looks like this:

		dst[index] = src[index] + table[index % table_size];

	If there is no table, the index modulo 256 is added instead.

	The source and destination values are taken as bytes.

SEE ALSO

	Decode

*/
void moCryptXOr::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size) {
				f_encode_index = 0;
			}
		}
		dst[i] = src[i] ^ v;
		i++;
	}
}






/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually substract the give values to each entry in the table.

	The source and destination values are taken as bytes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptXOr::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size) {
				f_decode_index = 0;
			}
		}
		dst[i] = src[i] ^ v;
		i++;
	}
}







/************************************************************ DOC:

CLASS

	moCryptXOr

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptXOr::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 1;
}






/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	Constructor - initializes an moCryptRor8 object

SYNOPSIS

	moCryptRor8(void);

DESCRIPTION

	This function initializes an moCryptRor8 object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptRor8::moCryptRor8(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptRor8 indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptRor8::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}




/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	GenerateTable - generate a random table

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function calls the RandomTable() base function.

SEE ALSO

	GenerateTable, RandomTable, CheckTableSize

*/
void moCryptRor8::GenerateTable(void)
{
	RandomTable();
}






/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by adding the values found
	in the table at a similar offset. The operation looks like this:

		dst[index] = src[index] + table[index % table_size];

	If there is no table, the index modulo 256 is added instead.

	The source and destination values are taken as bytes.

SEE ALSO

	Decode

*/
void moCryptRor8::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size) {
				f_encode_index = 0;
			}
		}
		v &= 7;
		dst[i] = (src[i] << v) | (src[i] >> (8 - v));
		i++;
	}
}






/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually substract the give values to each entry in the table.

	The source and destination values are taken as bytes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptRor8::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size) {
				f_decode_index = 0;
			}
		}
		v &= 7;
		dst[i] = (src[i] >> v)  | (src[i] << (8 - v));
		i++;
	}
}







/************************************************************ DOC:

CLASS

	moCryptRor8

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptRor8::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 1;
}






/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	Constructor - initializes an moCryptRor16 object

SYNOPSIS

	moCryptRor16(void);

DESCRIPTION

	This function initializes an moCryptRor16 object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptRor16::moCryptRor16(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptRor16 indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptRor16::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}




/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	GenerateTable - generate a random table

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function calls the RandomTable() base function.

SEE ALSO

	GenerateTable, RandomTable, CheckTableSize

*/
void moCryptRor16::GenerateTable(void)
{
	RandomTable();
}







/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by adding the values found
	in the table at a similar offset. The operation looks like this:

		dst[index] = src[index] + table[index % table_size];

	If there is no table, the index modulo 256 is added instead.

	The source and destination values are taken as bytes.

SEE ALSO

	Decode

*/
void moCryptRor16::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 2;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size) {
				f_encode_index = 0;
			}
		}
		v &= 15;
		reinterpret_cast<unsigned short *>(dst)[i] =
			(reinterpret_cast<const unsigned short *>(src)[i] << v)
				| (reinterpret_cast<const unsigned short *>(src)[i] >> (16 - v));
		i++;
	}

	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[src_size - 1];
	}
}






/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually substract the give values to each entry in the table.

	The source and destination values are taken as bytes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptRor16::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 2;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size) {
				f_decode_index = 0;
			}
		}
		v &= 15;
		reinterpret_cast<unsigned short *>(dst)[i] =
			(reinterpret_cast<const unsigned short *>(src)[i] >> v)
				| (reinterpret_cast<const unsigned short *>(src)[i] << (16 - v));
		i++;
	}

	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[src_size - 1];
	}
}







/************************************************************ DOC:

CLASS

	moCryptRor16

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptRor16::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 2;
}






/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	Constructor - initializes an moCryptRor32 object

SYNOPSIS

	moCryptRor32(void);

DESCRIPTION

	This function initializes an moCryptRor32 object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptRor32::moCryptRor32(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptRor32 indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptRor32::Reset(void)
{
	f_encode_index = 0;
	f_decode_index = 0;
}




/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	GenerateTable - generate a random table

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function calls the RandomTable() base function.

SEE ALSO

	GenerateTable, RandomTable, CheckTableSize

*/
void moCryptRor32::GenerateTable(void)
{
	RandomTable();
}








/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by adding the values found
	in the table at a similar offset. The operation looks like this:

		dst[index] = src[index] + table[index % table_size];

	If there is no table, the index modulo 256 is added instead.

	The source and destination values are taken as bytes.

SEE ALSO

	Decode

*/
void moCryptRor32::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 4;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_encode_index);
			f_encode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_encode_index];
			f_encode_index++;
			if(f_encode_index >= f_table_size) {
				f_encode_index = 0;
			}
		}
		v &= 31;
		if(v == 0) {
			// we need not to shift here since (a >> 32) is unknown on
			// most processors
			reinterpret_cast<unsigned long *>(dst)[i] =
				reinterpret_cast<const unsigned long *>(src)[i];
		}
		else {
			reinterpret_cast<unsigned long *>(dst)[i] =
				(reinterpret_cast<const unsigned long *>(src)[i] << v)
				| (reinterpret_cast<const unsigned long *>(src)[i] >> (32 - v));
		}
		i++;
	}

	if((dst_size & 2) != 0) {
		dst[(dst_size & -2) - 2] = src[(src_size & -2) - 2];
	}
	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[src_size - 1];
	}
}






/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually substract the give values to each entry in the table.

	The source and destination values are taken as bytes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptRor32::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;
	unsigned char	v;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	src_size /= 4;
	i = 0;
	while(i < src_size) {
		if(f_table == 0) {
			v = static_cast<unsigned char>(f_decode_index);
			f_decode_index++;
		}
		else {
			v = static_cast<unsigned char *>(f_table)[f_decode_index];
			f_decode_index++;
			if(f_decode_index >= f_table_size) {
				f_decode_index = 0;
			}
		}
		v &= 31;
		if(v == 0) {
			// we need not to shift here since (a >> 32) is unknown on
			// most processors
			reinterpret_cast<unsigned short *>(dst)[i] =
				reinterpret_cast<const unsigned short *>(src)[i];
		}
		else {
			reinterpret_cast<unsigned short *>(dst)[i] =
				(reinterpret_cast<const unsigned short *>(src)[i] >> v)
				| (reinterpret_cast<const unsigned short *>(src)[i] << (32 - v));
		}
		i++;
	}

	if((dst_size & 2) != 0) {
		dst[(dst_size & -2) - 2] = src[(src_size & -2) - 2];
	}
	if((dst_size & 1) != 0) {
		dst[dst_size - 1] = src[src_size - 1];
	}
}







/************************************************************ DOC:

CLASS

	moCryptRor32

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptRor32::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 2;
}






/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	Constructor - initializes an moCryptMix object

SYNOPSIS

	moCryptMix(void);

DESCRIPTION

	This function initializes an moCryptMix object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptMix::moCryptMix(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptMix indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptMix::Reset(void)
{
}



/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	CheckTableSize - change the size in multiple of unsigned long

SYNOPSIS

	virtual void CheckTableSize(void);

DESCRIPTION

	This function tansform the default size of the given table
	so it correspond to a table of unsigned long instead of
	unsigned char.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptMix::CheckTableSize(unsigned long& size)
{
	size *= sizeof(unsigned long);
}





/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	GenerateTable - generates a table to mix bytes

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function generates a table which can be used by the
	encoder and decoder. The size parameter needs to be the
	number of entries. The CheckTableSize() function will
	multiply that size by sizeof(unsigned long) as required.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptMix::GenerateTable(void)
{
	unsigned long		i, j, v;

	// create an ordered array
	i = f_table_size / sizeof(unsigned long);
	while(i > 0) {
		i--;
		reinterpret_cast<unsigned long *>(f_table)[i] = i;
	}
	// randomize the array now
	i = f_table_size / sizeof(unsigned long);
	while(i > 0) {
		i--;
		j = f_random->Random() % (f_table_size / sizeof(unsigned long));
		v = reinterpret_cast<unsigned long *>(f_table)[i];
		reinterpret_cast<unsigned long *>(f_table)[i] =
			reinterpret_cast<unsigned long *>(f_table)[j];
		reinterpret_cast<unsigned long *>(f_table)[j] = v;
	}
}





/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	NewTable - check new table validity

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function ensures the new table is valid by checking the offsets
	against the size of the table.

SEE ALSO

	SetTable

*/
void moCryptMix::NewTable(void)
{
	unsigned long		idx;

	// empty tables are always fine
	if(f_table == 0 || f_table_size == 0) {
		return;
	}

	idx = f_table_size / sizeof(unsigned long);
	if(idx * sizeof(unsigned long) != f_table_size) {
		throw moError(MO_ERROR_INVALID, "invalid table size - must be a multiple of %d bytes", sizeof(unsigned long));
	}

	while(idx > 0) {
		idx--;
		if(reinterpret_cast<unsigned long *>(f_table)[idx] >= f_table_size) {
			throw moError(MO_ERROR_INVALID, "invalid table - offset too large");
		}
	}
}




/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the source buffer by reordering bytes from
	a position to another. The table defines the destination position.
	The following is the expression used to move the bytes:

		dst[table[index]] = src[index];

	The size of the table must perfectly match the size of the source
	buffer otherwise the transformation doesn't happen. For this reason
	this encryption method can't be used after encryptions which generate
	variable sized entries (such as Compress and Noise).

SEE ALSO

	Decode

*/
void moCryptMix::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	if(f_table == 0		// we need a table with the proper size
	|| src_size != f_table_size / sizeof(unsigned long)) {
		// no table, no mix
		memcpy(dst, src, src_size);
		return;
	}

	i = 0;
	while(i < src_size) {
		dst[reinterpret_cast<unsigned long *>(f_table)[i]] = src[i];
		i++;
	}
}






/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the effect of a previous call to the Encode()
	function of the same object with the same table.

	It will actually place the bytes back where they are supposed to be
	with the following code:

		dst[index] = src[table[index]];

SEE ALSO

	Encode, DecodeSize

*/
void moCryptMix::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long	i;

	dst = new unsigned char[src_size];
	if(dst == 0) {
		return;
	}
	dst_size = src_size;
	if(f_table == 0		// we need a table with a proper size
	|| src_size != f_table_size / sizeof(unsigned long)) {
		// no table, no mix
		memcpy(dst, src, src_size);
		return;
	}

	i = 0;
	while(i < src_size) {
		dst[i] = src[reinterpret_cast<unsigned long *>(f_table)[i]];
		i++;
	}
}







/************************************************************ DOC:

CLASS

	moCryptMix

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.

SEE ALSO

	Encode, Decode

*/
long moCryptMix::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return f_table_size <= 0 ? 1 : f_table_size / sizeof(unsigned long);
}






/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	Constructor - initializes an moCryptCompress object

SYNOPSIS

	moCryptCompress(void);

DESCRIPTION

	This function initializes an moCryptCompress object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptCompress::moCryptCompress(void)
{
	f_info.version = MO_CRYPT_COMPRESS_VERSION;
	f_info.level = Z_DEFAULT_COMPRESSION;
	f_info.input_size = 0;

	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptCompress indexes.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptCompress::Reset(void)
{
}







/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	CheckTableSize - check the size of a GenerateTable() call

SYNOPSIS

	virtual void CheckTableSize(unsigned long& size);

DESCRIPTION

	By default, automatic compression table generation does
	nothing.

SEE ALSO

	GenerateTable

*/
void moCryptCompress::CheckTableSize(unsigned long& size)
{
	size = 0;
}




/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	NewTable - get data from new table

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function defines an internal information structure from the
	user defined table.

SEE ALSO

	SetTable

*/
void moCryptCompress::NewTable(void)
{
	moCryptCompressInfo	*p;

	f_info.version = MO_CRYPT_COMPRESS_VERSION;
	f_info.level = Z_DEFAULT_COMPRESSION;
	f_info.input_size = 0;
	if(f_table != 0 && f_table_size >= sizeof(long)) {
		p = reinterpret_cast<moCryptCompressInfo *>(f_table);
		switch(f_info.version = p->version) {
		case MO_CRYPT_COMPRESS_VERSION:
			f_info.level = p->level;
			f_info.input_size = p->input_size;
			break;

		}
	}
}





/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function compresses the source buffer using the z library.

	Note that small buffers are not always very well compressed. If
	you use many other encoding functions before (such as the Code
	which doubles the buffer) you have a greater chance to archieve
	a good level of compression.

	The table, when defined, will be used as a structure which defines
	parameters such as the level of compression and the input size.
	It is important to know the input size in order to decompress so
	the size will be saved within the resulting buffer unless the
	user knows that the input size will always be the same. In this
	case the table can define that size.

	When the size of the input is saved in the resulting buffer it is
	spread within the first 16 bytes. If the result represents less
	than 16 bytes, then random data is added at the end of the resulting
	buffer.

	The structure is defined as follow:

		#define	MO_CRYPT_COMPRESS_VERSION		1000

		struct moCryptCompressInfo {
			long			version;	// MO_CRYPT_COMPRESS_VERSION
			int			level;		// compression level
			unsigned long		input_size;	// if 0 - save input size in compressed buffer
		};

	The version will be used to ensure future compatibility with newer
	versions of the library.

	When no table is defined, the default level of compress is used
	and the input_size is assumed to vary (to be 0).

	Currently, the level of compression can be any value from -1 to +9.
	-1 is used as the default (Z_DEFAULT_COMPRESSION), 0 for "uncompress",
	1 as the worse and 9 as the best.

SEE ALSO

	Decode

*/
void moCryptCompress::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	uLongf			dest_length;
	int			r;

	if(f_info.input_size != 0 && f_info.input_size != src_size) {
		// if the input & source sizes don't match we can't decode
		// properly!
		return;
	}

	dest_length = src_size * 2 + 16;
	dst = new unsigned char[dest_length];
	if(dst == 0) {
		return;
	}

	// compress the source now
	r = compress2(static_cast<Bytef *>(dst), &dest_length, static_cast<const Bytef *>(src), src_size, f_info.level);
	if(r == Z_STREAM_ERROR) {
		r = compress2(static_cast<Bytef *>(dst), &dest_length, static_cast<const Bytef *>(src), src_size, Z_DEFAULT_COMPRESSION);
	}
	if(r != Z_OK) {
		// the compression failed
		delete [] dst;
		dst = 0;
		return;
	}

	// save the input size? (we do so when the input size can vary)
	if(f_info.input_size == 0) {
		while(dest_length < 16) {
			// TODO: use a better random() function!
			// NOTE: we won't have such small buffers ever!
			dst[dest_length] = rand();
			dest_length++;
		}
		memmove(dst + 10, dst + 9, dest_length - 9);
		dst[9] = static_cast<unsigned char>(src_size >> 16);
		memmove(dst +  7, dst + 6, dest_length - 5);
		dst[6] = static_cast<unsigned char>(src_size >> 24);
		memmove(dst +  5, dst + 4, dest_length - 2);
		dst[4] = static_cast<unsigned char>(src_size);
		memmove(dst +  2, dst + 1, dest_length + 2);
		dst[1] = static_cast<unsigned char>(src_size >> 8);
		dest_length += 4;
	}

	dst_size = dest_length;
}






/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function decompresses the source.

	If the size of the output (original buffer) then a proper table with
	a proper structure (see Encode() above) has to be defined and at least
	the input_size field has to be defined (and the version which will never
	be mandatory).

SEE ALSO

	Encode, DecodeSize

*/
void moCryptCompress::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	uLongf			dest_length;
	unsigned char *		s;
	int			r;

	s = 0;
	dst_size = f_info.input_size;
	if(dst_size == 0) {
		// the size of the decompressed buffer is present in the compressed
		// (source) buffer at specific positions
		s = new unsigned char[src_size];
		dst_size = (src[8] << 24) | (src[12] << 16) | (src[1] << 8) | src[5];
		*s = *src;
		memcpy(s + 1, src +  2, 3);
		memcpy(s + 4, src +  6, 2);
		memcpy(s + 6, src +  9, 3);
		memcpy(s + 9, src + 13, src_size - 13);
		src = s;
		src_size -= 4;		// very small buffers are not of that size
	}

	dst = new unsigned char[dst_size];
	if(dst == 0) {
		delete s;
		return;
	}

	// uncompress the source now
	dest_length = dst_size;
	r = uncompress(static_cast<Bytef *>(dst), &dest_length,
				static_cast<const Bytef *>(src), src_size);
	delete s;
	if(r != Z_OK || dest_length != dst_size) {
		// the compression failed
		delete [] dst;
		dst = 0;
		return;
	}
}







/************************************************************ DOC:

CLASS

	moCryptCompress

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.
	
	In the case of an moCryptCompress object it returns -1 meaning that the
	size of the input can't be infered. This may change in the future where
	some models will enable the user to use a compress as the last entry of
	its encryption chain with "automated streams readers."

SEE ALSO

	Encode, Decode

*/
long moCryptCompress::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return -1;
}







/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	Constructor - initializes an moCryptSwap object

SYNOPSIS

	moCryptSwap(void);

DESCRIPTION

	This function initializes an moCryptSwap object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptSwap::moCryptSwap(void)
{
	f_decode = 0;

	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	Destructor - cleans up an moCryptSwap object

SYNOPSIS

	~moCryptSwap(void);

DESCRIPTION

	This function cleans up an moCryptSwap object.

SEE ALSO

	Encode, Decode, Constructor

*/
moCryptSwap::~moCryptSwap(void)
{
	delete [] f_decode;
}




/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptSwap.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptSwap::Reset(void)
{
}






/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	NewTable - get data from new table

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function defines a corresponding decoding function.

SEE ALSO

	SetTable

*/
void moCryptSwap::NewTable(void)
{
	unsigned long	i;

	if(f_table_size != 256) {
		throw moError(MO_ERROR_INVALID, "a table for a moCryptSwap object must be 256 bytes");
	}

	delete [] f_decode;
	if(f_table_size > 0) {
		f_decode = new unsigned char[f_table_size];
		if(f_decode == 0) {
			throw moError(MO_ERROR_MEMORY, "out of memory");
		}
		i = f_table_size;
		while(i > 0) {
			i--;
			f_decode[f_table[i]] = static_cast<unsigned char>(i);
		}
	}
	else {
		f_decode = 0;
	}
}







/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	ChecTableSize - set the size of the table to 256

SYNOPSIS

	virtual void ChecTableSize(void);

DESCRIPTION

	This function ensures that the size of the table is exactly 256.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptSwap::CheckTableSize(unsigned long& size)
{
	size = 256;
}







/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	GenerateTable - generates a table to mix bytes

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function generates a table which can be used by the
	encoder and decoder. The size parameter needs to be the
	number of entries. The CheckTableSize() function will
	multiply that size by sizeof(unsigned long) as required.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptSwap::GenerateTable(void)
{
	unsigned long		i, j, v;

	// create an ordered array
	i = f_table_size;
	while(i > 0) {
		i--;
		f_table[i] = static_cast<unsigned char>(i);
	}
	// randomize the array now
	i = f_table_size;
	while(i > 0) {
		j = f_random->Random() % f_table_size;
		v = f_table[i];
		f_table[i] = f_table[j];
		f_table[j] = static_cast<unsigned char>(v);
	}
}






/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function exchange the bytes from the input with the bytes within
	a table (as in table[input]). For the Decode() process, an opposite
	table is created (see NewTable()) in order to restore the proper values.

	If no table is defined, nothing happens.

SEE ALSO

	Decode

*/
void moCryptSwap::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		i;

	dst_size = src_size;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}
	if(f_table == 0) {
		memcpy(dst, src, src_size);
		return;
	}

	i = dst_size;
	do {
		i--;
		dst[i] = f_table[src[i]];
	} while(i > 0);
}






/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the size added by the Encode() function.
	It is assumed that the size isn't necessary any more once this
	function was called.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptSwap::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		i;

	dst_size = src_size;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}
	if(f_decode == 0) {
		memcpy(dst, src, src_size);
		return;
	}

	i = dst_size;
	do {
		i--;
		dst[i] = f_decode[src[i]];
	} while(i > 0);
}







/************************************************************ DOC:

CLASS

	moCryptSwap

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.
	
	In the case of an moCryptSwap object it returns -1 meaning that the
	size of the input can't be infered. This may change in the future where
	some models will enable the user to use a compress as the last entry of
	its encryption chain with "automated streams readers."

SEE ALSO

	Encode, Decode

*/
long moCryptSwap::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 1;
}






/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	Constructor - initializes an moCryptNoise object

SYNOPSIS

	moCryptNoise(void);

DESCRIPTION

	This function initializes an moCryptNoise object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptNoise::moCryptNoise(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptNoise.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptNoise::Reset(void)
{
	f_encode_index = 0;
	f_encode_copy = 0;
	f_decode_index = 0;
	f_decode_copy = 0;
	f_decode_skip = false;
}





/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	GenerateTable - generates a table to mix bytes

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function generates a table of noise. The maximum
	number of unused words is 19 and the minimum is zero.

	The function expects that the Random() generator
	defined within the object won't only generate zeroes.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptNoise::GenerateTable(void)
{
	unsigned long		i;

	for(i = 0; i < f_table_size; i++) {
		f_table[i] = static_cast<unsigned char>(f_random->Random() % 20);
	}
}






/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	NewTable - check new table validity

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function ensures the new table is valid by making sure it includes
	values other than zeroes.

SEE ALSO

	SetTable

*/
void moCryptNoise::NewTable(void)
{
	long		i;

	if(f_table_size > 0 && f_table != 0) {
		i = f_table_size;
		while(i > 0) {
			i--;
			if(f_table[i] != 0) {
				return;
			}
		}
		throw moError(MO_ERROR_INVALID, "a table composed exclusively of zeroes is not acceptable");
	}
}






/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function adds bytes within a buffer at specified positions.

	The table is used as the number of bytes we should copy before
	another random value is inserted in the buffer. In this case the
	object expects a random object to be defined. Not having such an
	object will generate a throw.

	When no table is defined, then nothing is done to the source buffer
	and no random object is required.

	Using zeroes in the table enables multiple noise bytes to be
	inserted one after another.

	The table entries are taken as unsigned byte values.

ERRORS

	MO_ERROR_INVALID	happens when one tries to encode a buffer
				before to call SetRandom() with a valid
				random object

BUGS

	A table composed exclusively of zeroes is invalid since it would
	make the result an infinite buffer of noise. This is enforced
	when a table is defined.

SEE ALSO

	Decode

*/
void moCryptNoise::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		p, i, v, idx;

	// keep source buffer as is if no table defined
	if(f_table == 0) {
		dst_size = src_size;
		dst = new unsigned char[dst_size];
		if(dst != 0) {
			memcpy(dst, src, dst_size);
		}
		return;
	}

	// from here we need a random object
	if(f_random == 0) {
		throw moError(MO_ERROR_INVALID, "the moCryptNoise can't Encode() without a random object pointer defined with SetRandom()");
	}

	// do we have to add any noise in this buffer?
	if(f_encode_copy >= src_size) {
		f_encode_copy -= src_size;
		dst_size = src_size;
		if(f_encode_copy == 0) {
			dst_size++;
		}
		dst = new unsigned char[dst_size];
		if(dst != 0) {
			memcpy(dst, src, src_size);
			if(dst_size > src_size) {
				dst[src_size] = static_cast<unsigned char>(f_random->Random() >> 24);
			}
		}
		return;
	}

	// computes destination buffer size
	i = f_encode_copy;
	if(i == 0) {
		dst_size = 0;
	}
	else {
		dst_size = i + 1;
	}
	idx = f_encode_index;
	while(i < src_size) {
		v = f_table[idx];
		i += v;
		if(i > src_size) {
			v -= i - src_size + 1;
		}
		dst_size += v + 1;
		idx++;
		if(idx >= f_table_size) {
			idx = 0;
		}
	}

	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	i = f_encode_copy;
	if(i > 0) {
		// we close a previous call
		memcpy(dst, src, i);
		dst[i] = static_cast<unsigned char>(f_random->Random() >> 24);
		idx = i + 1;
	}
	else {
		// this is a new call, we already had the random value inserted
		idx = 0;
	}
	while(i < src_size) {
		v = f_table[f_encode_index];
		f_encode_copy = v;
		p = i;
		i += v;
		if(i > src_size) {
			v -= i - src_size;
		}
		memcpy(dst + idx, src + p, v);
		idx += v;
		f_encode_copy -= v;
		if(f_encode_copy == 0) {
			dst[idx] = static_cast<unsigned char>(f_random->Random() >> 24);
			idx++;
		}
		f_encode_index++;
		if(f_encode_index >= f_table_size) {
			f_encode_index = 0;
		}
	}
}






/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the noise added by the Encode() function.

	When used with only one byte as input, this function may return
	an empty buffer (if that's a byte of noise).

	There is no need for a random number generator definition in order
	to decode a buffer including noise.

BUGS

	It is possible that this Decode() function returns an empty destination
	buffer. This happens whenever the input is only one byte or many entries
	in the table are zeroes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptNoise::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		p, i, v, idx;

	if(f_table == 0) {
		dst = new unsigned char[src_size];
		if(dst == 0) {
			return;
		}
		dst_size = src_size;
		memcpy(dst, src, src_size);
		return;
	}

	if(f_decode_skip) {
		f_decode_skip = false;
		src++;
		src_size--;
		if(src_size == 0) {
			// case when we have to return an empty buffer
			dst = new unsigned char[1];
			dst_size = 0;
			return;
		}
	}

	if(f_decode_copy != 0 && f_decode_copy + 1 >= src_size) {
		f_decode_copy -= src_size;
		dst_size = src_size;
		f_decode_skip = f_decode_copy == 0;
		if(f_decode_copy == (unsigned long) -1) {
			// we skipped a byte of noise that we had here so we don't need
			// to skip it later!
			f_decode_copy = 0;
			dst_size--;
		}
		dst = new unsigned char[dst_size];
		if(dst != 0) {
			memcpy(dst, src, dst_size);
		}
		return;
	}

	dst_size = f_decode_copy;
	idx = f_decode_index;
	i = f_decode_copy + 1;
	if(i == 1) {
		i = 0;
	}
	while(i < src_size) {
		v = f_table[idx];
		i += v;
		if(i < src_size) {
			i++;
		}
		else if(i > src_size) {
			v -= i - src_size;
		}
		dst_size += v;
		idx++;
		if(idx >= f_table_size) {
			idx = 0;
		}
	}

	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	if(f_decode_copy == 0) {
		idx = 0;
		i = 0;
	}
	else {
		idx = f_decode_copy;
		i = idx + 1;
		memcpy(dst, src, idx);
		f_decode_copy = 0;
	}
	while(i < src_size) {		// always true the 1st time!
		v = f_table[f_decode_index];
		p = i;
		i += v;
		if(i < src_size) {
			memcpy(dst + idx, src + p, v);
			i++;
		}
		else if(i == src_size) {
			memcpy(dst + idx, src + p, v);
			f_decode_skip = true;
		}
		else {
			memcpy(dst + idx, src + p, src_size - p);
			f_decode_copy = i - src_size;
		}
		idx += v;	// this is "wrong" in the last case, but we are not
				// going to loop again anyway

		f_decode_index++;
		if(f_decode_index >= f_table_size) {
			f_decode_index = 0;
		}
	}
}







/************************************************************ DOC:

CLASS

	moCryptNoise

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.
	
	In the case of an moCryptNoise object it returns -1 meaning that the
	size of the input can't be infered. This may change in the future where
	some models will enable the user to use a compress as the last entry of
	its encryption chain with "automated streams readers."

SEE ALSO

	Encode, Decode

*/
long moCryptNoise::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 1;
}







/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	Constructor - initializes an moCryptCode object

SYNOPSIS

	moCryptCode(void);

DESCRIPTION

	This function initializes an moCryptCode object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptCode::moCryptCode(void)
{
	f_encode_count = 0;
	f_encode_index = 0;
	f_encode_array = 0;
	f_encode_code  = 0;

	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	Destructor - cleans up an moCryptCode object

SYNOPSIS

	~moCryptCode(void);

DESCRIPTION

	This function cleans up an moCryptCode object.

SEE ALSO

	Encode, Decode, Constructor

*/
moCryptCode::~moCryptCode(void)
{
	delete [] f_encode_count;
}





/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptCode.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptCode::Reset(void)
{
	if(f_encode_index != 0) {
		memset(f_encode_index, 0, sizeof(unsigned short) * 256);
	}
}





/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	CheckTableSize - change the size in multiple of unsigned long

SYNOPSIS

	virtual void CheckTableSize(void);

DESCRIPTION

	This function tansform the default size of the given table
	so it correspond to a table of unsigned long instead of
	unsigned char.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptCode::CheckTableSize(unsigned long& size)
{
	size = 65536;
}






/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	GenerateTable - generates a table to mix bytes

SYNOPSIS

	virtual void GenerateTable(void);

DESCRIPTION

	This function generates a table of noise. The maximum
	number of unused words is 19 and the minimum is zero.

	The function expects that the Random() generator
	defined within the object won't only generate zeroes.

SEE ALSO

	GenerateTable, CheckTableSize

*/
void moCryptCode::GenerateTable(void)
{
	unsigned long	count[256];
	long		i, j, k, l;
	unsigned char	*c;

	for(i = 0; i < 256; i++) {
		count[i] = 256;
	}
	for(i = 0; i < 512; i++) {
		j = f_random->Random() % 256;
		k = f_random->Random() % 256;
		l = f_random->Random() % 20 + 1;
		count[j] += l;
		count[k] -= l;
	}
	c = f_table;
	for(i = 0; i < 256; i++) {
		j = count[i];
		while(j > 0) {
			j--;
			*c++ = static_cast<unsigned char>(i);
		}
	}
	// the following is important to mix values between bytes
	for(i = 0; i < 65536; i++) {
		j = f_random->Random() % 65536;
		k = f_table[i];
		f_table[i] = f_table[j];
		f_table[j] = static_cast<unsigned char>(k);
	}
}







/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	NewTable - check new table validity

SYNOPSIS

	virtual void NewTable(void);

DESCRIPTION

	This function ensures the new table is valid by making sure it includes
	values other than zeroes.

SEE ALSO

	SetTable

*/
void moCryptCode::NewTable(void)
{
	long		i, j, cnt;
	unsigned short	*p;

	if(f_table_size > 0 && f_table != 0) {
		if(f_table_size != 65536 * sizeof(unsigned char)) {
			throw moError(MO_ERROR_INVALID, "the moCryptCode object expects a table of exactly 65536 unsigned shorts");
		}
		if(f_encode_count == 0) {
			// first time we need to initialize the pointers
			f_encode_count = new unsigned short[256 + 256 + 256 * sizeof(unsigned short *) / sizeof(unsigned short) + 65536];
			if(f_encode_count == 0) {
				throw std::bad_alloc();
			}
			f_encode_index = f_encode_count + 256;
			f_encode_array = reinterpret_cast<unsigned short **>(f_encode_index + 256);
			f_encode_code  = reinterpret_cast<unsigned short * >(f_encode_array + 256);
		}
		// 1st we count every entry - also we reset the indices
		memset(f_encode_count, 0, 256 * 2 * sizeof(unsigned short));
		i = 65536;
		while(i > 0) {
			i--;
			j = f_table[i];
			cnt = f_encode_count[j];
			if(cnt > 65281) {
				throw moError(MO_ERROR_INVALID, "the moCryptCode object can't handle tables with some unrepresented bytes");
			}
			f_encode_count[j] = static_cast<unsigned short>(cnt + 1);
		}
		// then generate the array of pointers to the f_encode_code entries
		p = f_encode_code;
		for(i = 0; i < 256; i++) {
			f_encode_array[i] = p;
			p += f_encode_count[i];
		}
		// now we can generate the encoding table
		i = 65536;
		while(i > 0) {
			i--;
			cnt = f_table[i];
			j = f_encode_index[cnt];
			f_encode_array[cnt][j] = static_cast<unsigned short>(i);
			f_encode_index[cnt] = static_cast<unsigned short>(j + 1);
		}
		Reset();

#if 0
for(i = 0; i < f_encode_count[0]; i++) {
printf("%04X ", f_encode_array[0][i]);
if((i & 15) == 15) printf("\n");
}
printf("\n");
#endif

	}
}






/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function encodes the input taken as an array of 8 bit values
	into an array of variable 16 bit values (i.e. many 16 bit values
	correspond to a single 8 bit value).

	All the 16 bits values will be saved in order within the encoding
	tables. In order to randomize their use, a random object should
	be used (see SetRandom()).

ERRORS

	MO_ERROR_INVALID	happens when there is no decoding table

BUGS

	A table composed exclusively of zeroes is invalid since it would
	make the result an infinite buffer of noise. This is enforced
	when a table is defined.

SEE ALSO

	Decode

*/
void moCryptCode::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		i, j, v, r;

	// keep source buffer as is if no table defined
	if(f_table == 0) {
		throw moError(MO_ERROR_INVALID, "the moCryptCode can't Encode() without a table object");
	}

	dst_size = src_size * 2;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	// we MUST generate the table from byte 0 to src_size otherwise
	// the f_random or f_encode_index[] values will not correspond
	for(i = 0; i < src_size; i++) {
		j = src[i];
		if(f_random == 0) {
			// use our indices to use the next 16 bit value
			v = f_encode_index[j];
			r = f_encode_array[j][v];
			v++;
			if(v >= f_encode_count[j]) {
				v = 0;
			}
			f_encode_index[j] = static_cast<unsigned short>(v);
		}
		else {
			// got a random object, select a random 16 bit value
			v = (f_random->Random() >> 16) % f_encode_count[j];
			r = f_encode_array[j][v];
		}
		reinterpret_cast<unsigned short *>(dst)[i] = static_cast<unsigned short>(r);
	}
}






/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the noise added by the Encode() function.

	When used with only one byte as input, this function may return
	an empty buffer (if that's a byte of noise).

	There is no need for a random number generator definition in order
	to decode a buffer including noise.

BUGS

	It is possible that this Decode() function returns an empty destination
	buffer. This happens whenever the input is only one byte or many entries
	in the table are zeroes.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptCode::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		i;

	if(f_table == 0) {
		throw moError(MO_ERROR_INVALID, "the moCryptCode can't Decode() without a table object");
	}

	if((src_size & 1) != 0) {
		throw moError(MO_ERROR_INVALID, "invalid size in moCryptCode::Decode()");
	}

	dst_size = src_size / 2;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	for(i = 0; i < dst_size; i++) {
		dst[i] = f_table[reinterpret_cast<const unsigned short *>(src)[i]];
	}
}







/************************************************************ DOC:

CLASS

	moCryptCode

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.
	
	In the case of an moCryptCode object it returns -1 meaning that the
	size of the input can't be infered. This may change in the future where
	some models will enable the user to use a compress as the last entry of
	its encryption chain with "automated streams readers."

SEE ALSO

	Encode, Decode

*/
long moCryptCode::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	return 2;
}






/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	Constructor - initializes an moCryptSize object

SYNOPSIS

	moCryptSize(void);

DESCRIPTION

	This function initializes an moCryptSize object.

SEE ALSO

	Encode, Decode, Destructor

*/
moCryptSize::moCryptSize(void)
{
	Reset();
}




/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	Reset - initializes counter

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	This function resets an moCryptSize.

SEE ALSO

	Encode, Decode, Destructor

*/
void moCryptSize::Reset(void)
{
}








/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	CheckTableSize - check the size of a GenerateTable() call

SYNOPSIS

	virtual void CheckTableSize(unsigned long& size);

DESCRIPTION

	The size object really doesn't require a table.

SEE ALSO

	GenerateTable

*/
void moCryptSize::CheckTableSize(unsigned long& size)
{
	size = 0;
}





/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	Encode - encodes the source buffer

SYNOPSIS

	virtual void Encode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function adds a size within the input buffer.

	A table can be specified in which case it is taken as an unsigned
	long word and represents the minimum expected size of the input
	data.

	This is used to encode the size in different ways and places.

	Otherwise the size is encoded on the first four bytes of the
	buffer.

SEE ALSO

	Decode

*/
void moCryptSize::Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		size;

	if(f_table != 0 && f_table_size >= sizeof(unsigned long)) {
		// we have a hint by the user
		size = * (unsigned long *) f_table;
	}
	else {
		size = 0;
	}

	dst_size = src_size + 4;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	if(size < 16) {
		// too small to mix the size (not really worth it either)
		memcpy(dst + 4, src, src_size);
		dst[0] = static_cast<unsigned char>(dst_size >> 24);
		dst[1] = static_cast<unsigned char>(dst_size >> 16);
		dst[2] = static_cast<unsigned char>(dst_size >>  8);
		dst[3] = static_cast<unsigned char>(dst_size);
	}
	else {
		throw moError("Function not implemented yet");
	}
}






/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	Decode - decodes the source buffer

SYNOPSIS

	virtual void Decode(const unsigned char *src, unsigned long src_size,
			unsigned char *& dst, unsigned long& dst_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer
	dst - a pointer to void set to point to the result
	dst_size - the size of the destination buffer

DESCRIPTION

	This function removes the size added by the Encode() function.
	It is assumed that the size isn't necessary any more once this
	function was called.

SEE ALSO

	Encode, DecodeSize

*/
void moCryptSize::Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size)
{
	unsigned long		size;

	if(f_table != 0 && f_table_size >= sizeof(unsigned long)) {
		// we have a hint by the user
		size = * (unsigned long *) f_table;
	}
	else {
		size = 0;
	}

	dst_size = src_size - 4;
	dst = new unsigned char[dst_size];
	if(dst == 0) {
		return;
	}

	if(size < 16) {
		memcpy(dst, src + 4, dst_size);
	}
	else {
		throw moError("Function not implemented yet");
	}
}







/************************************************************ DOC:

CLASS

	moCryptSize

NAME

	DecodeSize - minimum size to properly decode a source buffer

SYNOPSIS

	virtual long DecodeSize(const unsigned char *src, unsigned long src_size);

PARAMETERS

	src - source buffer
	src_size - size of the source buffer

DESCRIPTION

	This function returns the minimum number of bytes necessary in order to
	decode a buffer with this decoder.
	
	In the case of an moCryptSize object it returns -1 meaning that the
	size of the input can't be infered. This may change in the future where
	some models will enable the user to use a compress as the last entry of
	its encryption chain with "automated streams readers."

SEE ALSO

	Encode, Decode

*/
long moCryptSize::DecodeSize(const unsigned char *src, unsigned long src_size)
{
	unsigned long		size;

	if(f_table != 0 && f_table_size >= sizeof(unsigned long)) {
		// we have a hint by the user
		size = * (unsigned long *) f_table;
	}
	else {
		size = 0;
	}

	if((size >= 16 && src_size < size) || src_size < 4) {
		// not enough data to tell yet
		return 0;
	}

	// we can determine the right size now
	if(size < 16) {
		size = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
	}
	else {
		throw moError("Function not implemented yet");
	}

	return size;
}






}			// namespace molib;

// vim: ts=8 sw=8 syntax=cpp.doxygen

