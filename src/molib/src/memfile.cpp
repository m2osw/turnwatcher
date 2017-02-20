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
#pragma implementation "mo/mo_memfile.h"
#endif

#include	"mo/mo_memfile.h"



namespace molib
{

/************************************************************ DOC:

CLASS

	moMemFile

NAME

	Contructors - create a new file object
	Destructor - ensure the memory used by this object is released

SYNOPSIS

	moMemFile(void);
	virtual ~moMemFile();

DESCRIPTION

	The memory file object will be used whenever you don't have a
	really large file and the file is temporary. This file is
	based on the moIOStream and all the functions available in
	the moIOStream are functional with this file implementation.

SEE ALSO

	moBuffer class

*/
moMemFile::moMemFile(void)
{
}



moMemFile::~moMemFile()
{
}



/************************************************************ DOC:

CLASS

	moMemFile

NAME

	RawRead - read bytes from the memory file
	RawWrite - writes bytes to the memory file

SYNOPSIS

	virtual int RawRead(void *buffer, size_t length);
	virtual int RawWrite(const void *buffer, size_t length);

DESCRIPTION

	The RawRead() function tries to read length bytes from the
	current input position. The number of bytes read is returned.

	The RawWrite() function writes length bytes to the current
	output position in this memory file. This can't fail (unless
	not enough memory can be allocated) and thus this function
	always returns length.

RETURN VALUE

	Both functions return the number of bytes copied.

*/
int moMemFile::RawRead(void *buffer, size_t length)
{
	void		*data;
	unsigned long	size;

	f_buffer.Get(data, size);
	if(f_input_position + static_cast<uint32_t>(length) > size) {
		// the result of this can be 0
		length = size - f_input_position;
	}

	// length CAN be zero
	if(length > 0) {
		memcpy(buffer, reinterpret_cast<unsigned char *>(data) + f_input_position, length);	/* Flawfinder: ignore */
		f_input_position += static_cast<uint32_t>(length);
	}

	return static_cast<int>(length);
}


int moMemFile::RawWrite(const void *buffer, size_t length)
{
	f_buffer.Copy(f_output_position, buffer, static_cast<unsigned long>(length));
	f_output_position += static_cast<uint32_t>(length);

	return static_cast<int>(length);
}





/************************************************************ DOC:

CLASS

	moMemFile

NAME

	InputSize - the size of the memory files in bytes
	OutputSize - the size of the memory files in bytes

SYNOPSIS

	virtual size_t InputSize(void) const;
	virtual size_t OutputSize(void) const;

DESCRIPTION

	The InputSize() and OutputSize() functions both return
	the current size of the memory file in bytes.

	Whatever happens, the size of the input is always the
	same as the size of the output.

RETURN VALUE

	the size of the memory file in bytes

*/
size_t moMemFile::InputSize(void) const
{
	return (long) f_buffer;
}


size_t moMemFile::OutputSize(void) const
{
	return (long) f_buffer;
}


};			// namespace molib;

// vim: ts=8

