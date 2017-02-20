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
#pragma implementation "mo/mo_stream.h"
#endif

#include	"mo/mo_stream.h"


#ifndef MO_TEMPLATE_H
#include	"mo/mo_template.h"
#endif




namespace molib
{

/************************************************************ DOC:

CLASS

	moIStream

NAME

	Constructor - create an moIStream object
	Destructor - destroys an moIStream object

SYNOPSIS

	moIStream(void);
	virtual ~moIStream();

DESCRIPTION

	An moIStream is a pure class. It is necessary to have an inherited
	version of it to instanciate an actual stream.

	The moIStream is used for input. Please, use an moOStream for
	output purposes and an moIOStream for both input and output.

SEE ALSO

	moOStream

*/
moIStream::moIStream(void)
	: f_input_endian(BYTE_ORDER)		// use machine endian by default
{
	//f_input_position -- auto-init
	//f_input_filter -- auto-init
	f_input_filename = 0;
	f_input_unget_position = 0;
	//f_input_unget[] --- since the position is zero, there's nothing here!
	//f_secret -- auto-init
}

moIStream::~moIStream()
{
	if(f_secret) {
		// for safety we reset the unget buffer
		memset(f_input_unget, 0, sizeof(f_input_unget));
	}
	delete [] f_input_filename;
}



/************************************************************ DOC:

CLASS

	moIStream

NAME

	Read - call this function to read some data from a file
	RawRead - the implementation dependent read function

SYNOPSIS

	virtual int Read(void *buffer, size_t length);

	protected:
	virtual int RawRead(void *buffer, size_t length) = 0;

PARAMETERS

	buffer - a pointer where the data read will be saved
	length - the number of bytes to read in buffer

DESCRIPTION

	The Read() function will call the RawRead() and then pass the
	data to the current filter. It will then read the result out
	of the filter to the user buffer.

	The RawRead() function is implementation dependent and thus
	is a pure virtual here. The moFile, for instance, defines the
	RawRead() as the fread() function in some input disk file or
	some other simple character stream.

	If the Unread() function was succesfully called prior to a
	Read(), then the unread bytes are returned first. Note that
	Unread() can't "de-filter" data and thus the filter won't
	be run on data read from the internal buffer used to support
	the Unread() function.

	Please, read the BUGS section of the Unread() function for
	more information about how to unread data previously read.

RETURN VALUE

	the number of bytes read
	0 when nothing can be read (eventually EOF was reached)
	-1 when an error occurs
 
SEE ALSO

	Get(), Unread(), Unget()

*/
int moIStream::Read(void *buffer, size_t length)
{
	int		l, p, total;
	char		buf[BUFSIZ];	/* Flawfinder: ignore */

	if(length == 0) {
		return 0;
	}

	total = 0;

	if(f_input_unget_position > 0UL) {
		l = static_cast<int>(moMin(length, f_input_unget_position));
		length -= l;
		total = l;		// WARNING: here we know that total is 0

		// we need to read the unget buffer backward
		p = 0;
		while(l > 0) {
			l--;
			f_input_unget_position--;
			static_cast<unsigned char *>(buffer)[p] = f_input_unget[f_input_unget_position];
			p++;
		}

		if(length == 0) {
			return total;
		}
		buffer = static_cast<unsigned char *>(buffer) + total;
	}

	if(!f_input_filter) {
		l = RawRead(buffer, length);

//fprintf(stderr, "moIStream::Read(1) read %d chars from input_filter...\n", l);

		if(l > 0) {
			total += l;
		}
		return total > 0 || l == 0 ? total : -1;
	}

	for(;;) {
		l = f_input_filter->Read(buffer, static_cast<unsigned long>(length));

//fprintf(stderr, "moIStream::Read(2) read %d chars from input_filter... (%08lX)\n", l, * (long *) buffer);

		if(l < 0) {
			return -1;
		}
		total += l;
		length -= l;
		if(length == 0) {
			return total;
		}
		// we need more raw data
		l = RawRead(buf, moMin(f_input_filter->FreeSpace(), sizeof(buf)));
		if(l < 0) {
			// we may want to check for a non-blocking device
			// error; if such an error occurs, we want to
			// return total instead of -1
			return -1;
		}
		if(l == 0) {
			// we either reached the end of the input stream
			// or it is a non-blocking device
			return total;
		}
		p = f_input_filter->Write(buf, l);

//fprintf(stderr, "moIStream::Read(3) written %d chars to input_filter instead of %d (%08lX)\n", p, l, * (long *) buffer);

	}
	/*NOTREACHED*/
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	Unread - write in the unget buffer

SYNOPSIS

	int Unread(const void *buffer, size_t length);

PARAMETERS

	buffer - a pointer to the data to unread
	length - the number of bytes to unread from buffer

DESCRIPTION

	The Unread() function can be used to push bytes back in
	the stream. The next time the Read() function is called,
	these bytes are read first.

	The function will throw an error if the number of bytes
	to be unread is too large for the internal buffer.

RETURN VALUE

	the number of bytes unread

BUGS

	The bytes are copied as is in the internal buffer. This
	means if you unread a long, it is copied as four bytes
	as given in the buffer. Reading this value as two
	shorts may therefore not yield the expected result.

	In other words, if you want to cancel a series of
	Get(char), you need to put the chars in reverse
	order in your buffer and then call the Unread()
	function at once:

	myfunc(moIStream *stream)
	{
		char		buf[32];
		char		*s;

		s = buf + sizeof(buf) / sizeof(buf[0]);
		for(i = 0; i < 5; i++) {
			s--;
			// NOTE: we should handle errors too!
			stream->Get(s);
		}
		if(strcmp(s, "ABCDE") == 0) {
			// do something
		}
		else {
			// cancel these read
			stream->Unread(s, 5);
		}
	}

	Note that this function can be used for chars but not
	for short, longs, etc.

SEE ALSO

	Get(), Read(), Unget()

*/
int moIStream::Unread(const void *buffer, size_t length)
{
	size_t		l;

	if(length > MAX_UNGET_SIZE - f_input_unget_position) {
		throw moError(MO_ERROR_OVERFLOW, "Too many Unread() called on input stream");
	}

	// we need to go backward with the unget buffer
	l = length;
	while(l > 0) {
		l--;
		f_input_unget[f_input_unget_position] = static_cast<const unsigned char *>(buffer)[l];
		f_input_unget_position++;
	}

	return static_cast<int>(length);
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	InputSize - the current size in bytes

SYNOPSIS

	virtual size_t InputSize(void) const;

DESCRIPTION

	The InputSize() function retrieves the current size of this
	input stream.

RETURN VALUE

	the size in bytes
	-1 if an error occur

SEE ALSO

	Read(), Get()

*/
size_t moIStream::InputSize(void) const
{
	return 0;
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	Get - read a value from the input stream

SYNOPSIS

	virtual int Get(<type>& c);

	where <type> is any one of the C++ integer and floating point
	type (bool, char, wchar_t, short, int, long, long long, float,
	double, long double -- integers have signed and an unsigned
	versions.)

PARAMETERS

	c - a reference where the data read is written

DESCRIPTION

	The Get() functions will read the necessary number of bytes
	from the input stream using the Read() function and return the
	resulting value. By default, the endian of the input stream
	is assumed to be the same as the system being used. It can
	be changed with the SetInputEndianess() function.

	Though these are defined as virtual, they don't need to be
	overridden in your own stream declaration if it isn't to make
	it any faster.

RETURN VALUE

	the number of bytes read (1, 2, 4 or 8 at this time)
	-1 when an error occurs

BUGS

	On some systems the size of some C++ types won't be properly
	recognized and thus an error will be thrown. This should never
	happen though.

SEE ALSO

	Unget(), moOStream::Put(), SetInputEndianess()

*/
int moIStream::Get(bool& c)
{
	int		r;
	bool*		p_c = &c;

	r = Read(p_c, sizeof(bool));
	if(r != sizeof(bool)) {
		if(r > 0) {
			Unread(p_c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER && sizeof(bool) != 1) {
		if(sizeof(bool) == 2) {
			uint16_t *ptr = reinterpret_cast<uint16_t *>(p_c);
			*ptr = moSwap16Bits(*ptr);
		}
		else if(sizeof(bool) == 4) {
			uint32_t *ptr = reinterpret_cast<uint32_t *>(p_c);
			*ptr = moSwap32Bits(*ptr);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'bool'.\n");
		}
	}

	return r;
}


int moIStream::Get(signed char& c)
{
	int		r;

	r = Read(&c, sizeof(signed char));
	if(r != sizeof(signed char)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	return r;
}


int moIStream::Get(unsigned char& c)
{
	int		r;

	r = Read(&c, sizeof(unsigned char));
	if(r != sizeof(unsigned char)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	return r;
}


int moIStream::Get(wchar_t& c)
{
	int		r;

	r = Read(&c, sizeof(wchar_t));
	if(r != sizeof(wchar_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(wchar_t) == 2) {
			c = moSwap16Bits((uint16_t) c);
		}
		else if(sizeof(wchar_t) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'wchar_t'.\n");
		}
	}

	return r;
}


int moIStream::Get(int16_t& c)
{
	int		r;

	r = Read(&c, sizeof(int16_t));
	if(r != sizeof(int16_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return r;
}


int moIStream::Get(uint16_t& c)
{
	int		r;

	r = Read(&c, sizeof(uint16_t));
	if(r != sizeof(uint16_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return r;
}


int moIStream::Get(int32_t& c)
{
	int		r;

	r = Read(&c, sizeof(int32_t));
	if(r != sizeof(int32_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return r;
}


int moIStream::Get(uint32_t& c)
{
	int		r;

	r = Read(&c, sizeof(uint32_t));
	if(r != sizeof(uint32_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return r;
}

#if __WORDSIZE == 32
int moIStream::Get(signed long& c)
{
	int		r;

	r = Read(&c, sizeof(signed long));

//fprintf(stderr, "moIStream::Read(signed long&) = %d\n", r);

	if(r != sizeof(signed long)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(signed long) == 4) {
			c = moSwap32Bits((int32_t) c);
		}
		else if(sizeof(signed long) == 8) {
			c = moSwap64Bits((int64_t) c);
		}
		else {
			throw moError("FATAL ERROR: moIStream: don't know how to swap a 'long'.\n");
		}
	}

	return r;
}


int moIStream::Get(unsigned long& c)
{
	int		r;

	r = Read(&c, sizeof(unsigned long));
	if(r != sizeof(unsigned long)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(unsigned long) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else if(sizeof(unsigned long) == 8) {
			c = moSwap64Bits((uint64_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'long'.\n");
		}
	}

	return r;
}
#endif

int moIStream::Get(int64_t& c)
{
	int		r;

	r = Read(&c, sizeof(int64_t));
	if(r != sizeof(int64_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return r;
}


int moIStream::Get(uint64_t& c)
{
	int		r;

	r = Read(&c, sizeof(uint64_t));
	if(r != sizeof(uint64_t)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return r;
}


int moIStream::Get(float& c)
{
	int		r;
	float*		p_c = &c;

	r = Read(p_c, sizeof(float));
	if(r != sizeof(float)) {
		if(r > 0) {
			Unread(p_c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		int32_t *ptr = reinterpret_cast<int32_t *>(p_c);
		*ptr = moSwap32Bits(*ptr);
	}

	return r;
}


int moIStream::Get(double& c)
{
	int		r;
	double*		p_c = &c;

	r = Read(p_c, sizeof(double));
	if(r != sizeof(double)) {
		if(r > 0) {
			Unread(p_c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		int64_t *ptr = reinterpret_cast<int64_t *>(p_c);
		*ptr = moSwap64Bits(*ptr);
	}

	return r;
}


#ifndef MO_CONFIG_NO_LONG_DOUBLE
int moIStream::Get(long double& c)
{
	int		r;
	int64_t		a, b;

	r = Read(&c, sizeof(long double));
	if(r != sizeof(long double)) {
		if(r > 0) {
			Unread(&c, r);
			return 0;
		}
		return r;
	}

	if(f_input_endian != BYTE_ORDER) {
		a = moSwap64Bits(((int64_t *) &c)[0]);
		b = moSwap64Bits(((int64_t *) &c)[1]);
		((int64_t *) &c)[0] = b;
		((int64_t *) &c)[1] = a;
	}

	return r;
}
#endif


/************************************************************ DOC:

CLASS

	moIStream

NAME

	Unget - put a value back in the stream

SYNOPSIS

	virtual int Unget(<type> c);

	where <type> is any one of the C++ integer and floating point
	type (bool, char, wchar_t, short, int, long, long long, float,
	double, long double -- integers have signed and an unsigned
	versions.)

PARAMETERS

	c - the value to unget

DESCRIPTION

	The Unget() functions will write the specified value back
	into the stream as if it had been read with the corresponding
	Get() function.

	The endianness will be determined as the currently running
	processor endian or the one defined by the user with
	the SetInputEndian().

	Though these are defined as virtual, they don't need to be
	overridden in your own stream declaration.

RETURN VALUE

	the number of bytes written back (1, 2, 4 or 8 at this time)
	-1 when an error occurs

BUGS

	It is possible that these functions will throw an error if
	the size of a C++ type can't properly be determined.

	You should Get() back the same way you Unget() data otherwise
	you may get an unexpected result due to the order in which
	bytes are re-read vs. unread. Please, see the BUGS section
	of the Unread() function for more information.

SEE ALSO

	Get(), moOStream::Put(), SetInputEndianess(), Unread()

*/
int moIStream::Unget(bool c)
{
	bool* p_c = &c;
	if(f_input_endian != BYTE_ORDER && sizeof(bool) != 1) {
		if(sizeof(bool) == 2) {
			uint16_t *ptr = reinterpret_cast<uint16_t *>(p_c);
			*ptr = moSwap16Bits(*ptr);
		}
		else if(sizeof(bool) == 4) {
			uint32_t *ptr = reinterpret_cast<uint32_t *>(p_c);
			*ptr = moSwap32Bits(*ptr);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'bool'.\n");
		}
	}

	return Unread(p_c, sizeof(bool));
}


int moIStream::Unget(signed char c)
{
	return Unread(&c, sizeof(signed char));
}


int moIStream::Unget(unsigned char c)
{
	return Unread(&c, sizeof(unsigned char));
}


int moIStream::Unget(wchar_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(wchar_t) == 2) {
			c = moSwap16Bits((uint16_t) c);
		}
		else if(sizeof(wchar_t) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'wchar_t'.\n");
		}
	}

	return Unread(&c, sizeof(wchar_t));
}


int moIStream::Unget(int16_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return Unread(&c, sizeof(int16_t));
}


int moIStream::Unget(uint16_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return Unread(&c, sizeof(uint16_t));
}


int moIStream::Unget(int32_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return Unread(&c, sizeof(int32_t));
}


int moIStream::Unget(uint32_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return Unread(&c, sizeof(uint32_t));
}


#if __WORDSIZE == 32
int moIStream::Unget(signed long c)
{
	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(signed long) == 4) {
			c = moSwap32Bits((int32_t) c);
		}
		else if(sizeof(signed long) == 8) {
			c = moSwap64Bits((int64_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'long'.\n");
		}
	}

	return Unread(&c, sizeof(signed long));
}


int moIStream::Unget(unsigned long c)
{
	if(f_input_endian != BYTE_ORDER) {
		if(sizeof(unsigned long) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else if(sizeof(unsigned long) == 8) {
			c = moSwap64Bits((uint64_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'long'.\n");
		}
	}

	return Unread(&c, sizeof(unsigned long));
}
#endif


int moIStream::Unget(int64_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return Unread(&c, sizeof(int64_t));
}


int moIStream::Unget(uint64_t c)
{
	if(f_input_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return Unread(&c, sizeof(uint64_t));
}


int moIStream::Unget(float c)
{
	float* p_c = &c;
	if(f_input_endian != BYTE_ORDER) {
		// we assume that float is 32 bits
		int32_t *ptr = reinterpret_cast<int32_t *>(p_c);
		*ptr = moSwap32Bits(*ptr);
	}

	return Unread(p_c, sizeof(float));
}


int moIStream::Unget(double c)
{
	double* p_c = &c;
	if(f_input_endian != BYTE_ORDER) {
		// we assume that double is 64 bits
		int64_t *ptr = reinterpret_cast<int64_t *>(p_c);
		*ptr = moSwap64Bits(*ptr);
	}

	return Unread(p_c, sizeof(double));
}


#ifndef MO_CONFIG_NO_LONG_DOUBLE
int moIStream::Unget(long double c)
{
	int64_t		a, b;
	long double *	p_c = &c;

	if(f_input_endian != BYTE_ORDER) {
		// we assume that double is 64 bits
		a = moSwap64Bits(reinterpret_cast<int64_t *>(p_c)[0]);
		b = moSwap64Bits(reinterpret_cast<int64_t *>(p_c)[1]);
		reinterpret_cast<int64_t *>(p_c)[0] = b;
		reinterpret_cast<int64_t *>(p_c)[1] = a;
	}

	return Unread(p_c, sizeof(long double));
}
#endif


/************************************************************ DOC:

CLASS

	moIStream

NAME

	InputFilename - get or change the input filename

SYNOPSIS

	virtual void InputFilename(const char *filename);
	virtual const char *InputFilename(void) const;

PARAMETER

	filename - a UTF-8 filename

DESCRIPTION

	A stream, by itself, doesn't require a name. If it is a named
	file, however, it can be given a name using this function.

RETURNS

	the current input stream filename

SEE ALSO

	Get(), SetInputEndianess(), moOStream::SetOutputEndianess()

*/
void moIStream::InputFilename(const char *filename)
{
	size_t		l;

	delete [] f_input_filename;
	f_input_filename = 0;

	if(filename != 0 && *filename != '\0') {
		l = strlen(filename) + 1;		/* Flawfinder: ignore */
		f_input_filename = new char[l];
		memcpy(f_input_filename, filename, l);	/* Flawfinder: ignore */
	}
}



const char *moIStream::InputFilename(void) const
{
	return f_input_filename;
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	SetInputEndianess - change the input endianess

SYNOPSIS

	virtual int SetInputEndianess(int endian);

PARAMETER

	endian - one of BIG_ENDIAN or LITTLE_ENDIAN

DESCRIPTION

	The different Get() functions which read multi-byte values
	often require a specific endian to be used. This can easilly
	be archived by forcing the input stream to a specific
	endianess.

RETURNS

	the previous endianess

SEE ALSO

	Get(), SetInputEndianess(), moOStream::SetOutputEndianess()

*/
int moIStream::SetInputEndianess(int endian)
{
	int		old_endian;

	old_endian = f_input_endian;
	f_input_endian = endian;

	return old_endian;
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	SetInputFilter - change the input filter

SYNOPSIS

	virtual moFIFOSPtr SetInputFilter(moFIFO *filter);

PARAMETER

	filter - a FIFO to be used as a data filter

DESCRIPTION

	This function accepts a FIFO which will be used as a filter to
	transform the raw data (read from the raw device) before to
	transfer them to the used of the stream.

	You can stop the filtering by setting the filter pointer to 0.

NOTES

	The default FIFO definition is transparent to the data (it isn't
	transformed) and this is isn't necessary to use such a FIFO on
	a stream.

RETURN VALUE

	The previous filter or 0 when no filter was installed.

SEE ALSO

	moOStream::SetOutputFilter()

*/
moFIFOSPtr moIStream::SetInputFilter(moFIFO *filter)
{
	moFIFOSPtr	old_filter;

	old_filter = f_input_filter;
	f_input_filter = filter;

	return old_filter;
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	ReadPosition - get and set the position where data will be read next

SYNOPSIS

	virtual size_t ReadPosition(void) const;
	virtual size_t ReadPosition(size_t position);

DESCRIPTION

	The ReadPosition() functions will be used to seek the input file
	to the position where data needs to be read.

RETURN VALUE

	the position where the pointer was before this call

SEE ALSO

	Read(), Get()

*/
size_t moIStream::ReadPosition(void) const
{
	return f_input_position;
}


size_t moIStream::ReadPosition(size_t position)
{
	size_t		old_position;

	old_position = f_input_position;
	f_input_position = static_cast<uint32_t>(position);

	return old_position;
}




/************************************************************ DOC:

CLASS

	moIStream

NAME

	Secret - mark the input stream as containing secret(s)

SYNOPSIS

	void Secret(void);

DESCRIPTION

	The moIStream can hold some of the input in an unget buffer.
	When marked as secret, that buffer will be cleared before
	the moIStream is deleted. Note that is may not be necessary
	to set this flag if the input is encrypted.

	Use this flag for security reasons.

	This flag is set to false by default.

SEE ALSO

	Unget(), Unread()

*/
void moIStream::Secret(void)
{
	f_secret = true;
}


/************************************************************ DOC:

CLASS

	moOStream

NAME

	Constructor - create an moOStream object
	Destructor - destroys an moOStream object

SYNOPSIS

	moOStream(void);
	virtual ~moOStream();

DESCRIPTION

	An moOStream is a pure class. It is necessary to have an inherited
	version of it in order to instanciate such a stream.

	The moOStream is used for output. Please, use an moIStream for
	input purposes and an moIOStream for both input and output.

SEE ALSO

	moIStream

*/
moOStream::moOStream(void)
	: f_output_endian(BYTE_ORDER)		// use this machine endian by default
{
	//f_output_position -- auto-init
	//f_output_filter -- auto-init
	f_output_filename = 0;
}

moOStream::~moOStream()
{
	delete [] f_output_filename;
}



/************************************************************ DOC:

CLASS

	moOStream

NAME

	Write - call this function to write some data from a file
	RawWrite - the implementation dependent write function

SYNOPSIS

	virtual int Write(const void *buffer, size_t length);

	protected:
	virtual int RawWrite(const void *buffer, size_t length) = 0;

PARAMETERS

	buffer - a pointer where the data read will be saved
	length - the number of bytes to read in buffer

DESCRIPTION

	The Write() function will call the RawWrite() after it passed the
	user data to the current output filter. The filter is in charge
	of transforming the user data in a raw format to be saved in the
	stream.

	The RawWrite() function is implementation dependent and thus
	is a pure virtual here. The moFile, for instance, defines the
	RawWrite() as the fwrite() function in some input disk file or
	some other simple character stream.

NOTES

	Because a filter may shrink (a compressor for instance) or
	enlarge (UTF8 to UTF32 for instance) the returned value
	could be interpreted in different ways. It was therefore
	decided that the number of bytes read from the user buffer
	pointer was the number we needed to return. This means
	more or less bytes may actually have been written to the
	final output stream.

RETURN VALUE

	the number of bytes from buffer written
	0 when nothing can be written
	-1 when an error occurs
 
SEE ALSO

	Put()

*/
int moOStream::Write(const void *buffer, size_t length)
{
	int		l, total, sz;
	char		buf[BUFSIZ];	/* Flawfinder: ignore */

	if(!f_output_filter) {
		return RawWrite(buffer, length);
	}

	total = 0;
	do {
		l = f_output_filter->Write(buffer, moMin(f_output_filter->FreeSpace(), length));
		if(l < 0) {
			return -1;
		}
		buffer = static_cast<const char *>(buffer) + l;
		length -= l;
		for(;;) {
			sz = f_output_filter->Read(buf, sizeof(buf));
			if(sz <= 0) {
				// not enough data for this transform filter
				break;
			}
			sz = RawWrite(buf, sz);
			if(sz < 0) {
				return -1;
			}
		}
		total += l;
	} while(length != 0);

	return total;
}




/************************************************************ DOC:

CLASS

	moOStream

NAME

	Put - write a value to the output stream

SYNOPSIS

	virtual int Put(<type> c);

	where <type> is any one of the C++ integer and floating point
	type (bool, char, wchar_t, short, int, long, long long, float,
	double, long double -- integers have signed and an unsigned
	versions.)

PARAMETERS

	c - the value to write

DESCRIPTION

	The Put() functions will write the necessary number of bytes
	from the given user parameter to the output using the Write()
	function. By default, the endian of the input stream
	is assumed to be the same as the system being used. It can
	be changed with the SetInputEndianess() function.

	Though these are defined as virtual, they don't need to be
	defined in your own stream declaration.

RETURN VALUE

	the number of bytes written (1, 2, 4 or 8 at this time)
	-1 when an error occurs
 
SEE ALSO

	moIStream::Get(), SetOututEndianess()

*/
int moOStream::Put(bool c)
{
	bool* p_c = &c;
	if(f_output_endian != BYTE_ORDER && sizeof(bool) != 1) {
		if(sizeof(bool) == 2) {
			uint16_t *ptr = reinterpret_cast<uint16_t *>(p_c);
			*ptr = moSwap16Bits(*ptr);
		}
		else if(sizeof(bool) == 4) {
			uint32_t *ptr = reinterpret_cast<uint32_t *>(p_c);
			*ptr = moSwap32Bits(*ptr);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'bool'.\n");
		}
	}

	return Write(p_c, sizeof(bool));
}


int moOStream::Put(signed char c)
{
	return Write(&c, sizeof(signed char));
}


int moOStream::Put(unsigned char c)
{
	return Write(&c, sizeof(unsigned char));
}


int moOStream::Put(wchar_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		if(sizeof(wchar_t) == 2) {
			c = moSwap16Bits((uint16_t) c);
		}
		else if(sizeof(wchar_t) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'wchar_t'.\n");
		}
	}

	return Write(&c, sizeof(wchar_t));
}


int moOStream::Put(int16_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return Write(&c, sizeof(int16_t));
}


int moOStream::Put(uint16_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap16Bits(c);
	}

	return Write(&c, sizeof(uint16_t));
}


int moOStream::Put(int32_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return Write(&c, sizeof(int32_t));
}


int moOStream::Put(uint32_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap32Bits(c);
	}

	return Write(&c, sizeof(uint32_t));
}


#if __WORDSIZE == 32
int moOStream::Put(signed long c)
{
	if(f_output_endian != BYTE_ORDER) {
		if(sizeof(signed long) == 4) {
			c = moSwap32Bits((int32_t) c);
		}
		else if(sizeof(signed long) == 8) {
			c = moSwap64Bits((int64_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'long'.\n");
		}
	}

	return Write(&c, sizeof(signed long));
}


int moOStream::Put(unsigned long c)
{
	if(f_output_endian != BYTE_ORDER) {
		if(sizeof(unsigned long) == 4) {
			c = moSwap32Bits((uint32_t) c);
		}
		else if(sizeof(unsigned long) == 8) {
			c = moSwap64Bits((uint64_t) c);
		}
		else {
			throw moError("FATAL ERROR: don't know how to swap a 'long'.\n");
		}
	}

	return Write(&c, sizeof(unsigned long));
}
#endif


int moOStream::Put(int64_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return Write(&c, sizeof(int64_t));
}


int moOStream::Put(uint64_t c)
{
	if(f_output_endian != BYTE_ORDER) {
		c = moSwap64Bits(c);
	}

	return Write(&c, sizeof(uint64_t));
}


int moOStream::Put(float c)
{
	float* p_c = &c;
	if(f_output_endian != BYTE_ORDER) {
		// we assume that float is 32 bits
		int32_t *ptr = reinterpret_cast<int32_t *>(p_c);
		*ptr = moSwap32Bits(*ptr);
	}

	return Write(p_c, sizeof(float));
}


int moOStream::Put(double c)
{
	double* p_c = &c;
	if(f_output_endian != BYTE_ORDER) {
		// we assume that double is 64 bits
		int64_t *ptr = reinterpret_cast<int64_t *>(p_c);
		*ptr = moSwap64Bits(*ptr);
	}

	return Write(p_c, sizeof(double));
}


#ifndef MO_CONFIG_NO_LONG_DOUBLE
int moOStream::Put(long double c)
{
	int64_t		a, b;

	if(f_output_endian != BYTE_ORDER) {
		// we assume that double is 64 bits
		long double *ptr = &c;
		a = moSwap64Bits(reinterpret_cast<int64_t *>(ptr)[0]);
		b = moSwap64Bits(reinterpret_cast<int64_t *>(ptr)[1]);
		reinterpret_cast<int64_t *>(ptr)[0] = b;
		reinterpret_cast<int64_t *>(ptr)[1] = a;
	}

	return Write(&c, sizeof(long double));
}
#endif


/************************************************************ DOC:

CLASS

	moOStream

NAME

	OutputFilename - get or change the input filename

SYNOPSIS

	virtual void OutputFilename(const char *filename);
	virtual const char *OutputFilename(void) const;

PARAMETER

	filename - a UTF-8 filename

DESCRIPTION

	Changes the name of the output stream. By default, a stream is
	not specific to a file and thus isn't named. If you create a
	named file, set the name using this function.

RETURNS

	a pointer to the filename

SEE ALSO

	moIStream::InputFilename()

*/
void moOStream::OutputFilename(const char *filename)
{
	size_t		l;

	delete [] f_output_filename;
	f_output_filename = 0;

	if(filename != 0 && *filename != '\0') {
		l = strlen(filename) + 1;		/* Flawfinder: ignore */
		f_output_filename = new char[l];
		memcpy(f_output_filename, filename, l);	/* Flawfinder: ignore */
	}
}



const char *moOStream::OutputFilename(void) const
{
	return f_output_filename;
}




/************************************************************ DOC:

CLASS

	moOStream

NAME

	SetOutputEndianess - change the output endianess

SYNOPSIS

	virtual int SetOutputEndianess(int endian);

PARAMETER

	endian - one of BIG_ENDIAN or LITTLE_ENDIAN

DESCRIPTION

	The different Put() functions which write multi-byte values
	often require a specific endian to be used. This can easilly
	be archived by forcing the output stream to a specific
	endianess.

RETURN VALUE

	the previous endianess

SEE ALSO

	Put(), moOStream::SetInputEndianess()

*/
int moOStream::SetOutputEndianess(int endian)
{
	int		old_endian;

	old_endian = f_output_endian;
	f_output_endian = endian;

	return old_endian;
}



/************************************************************ DOC:

CLASS

	moOStream

NAME

	SetOutputFilter - change the output endianess

SYNOPSIS

	virtual moFIFOSPtr SetOutputFilter(moFIFO *filter);

PARAMETER

	filter - a FIFO defined as a filter

DESCRIPTION

	The data sent to a streams can automatically be filtered. This is
	done by using a FIFO that you install on an output stream with
	the SetOutputFilter() function.

	Any data written to the stream will first be sent to the filter.

	It is possible to stop the filtering by setting the filter pointer
	to 0.

NOTES

	The default FIFO definition is transparent to the data (it isn't
	transformed) and this is isn't necessary to use such a FIFO on
	a stream.

RETURN VALUE

	The previous filter pointer or 0 when none was defined.

SEE ALSO

	moOStream::SetInputFilter()

*/
moFIFOSPtr moOStream::SetOutputFilter(moFIFO *filter)
{
	moFIFOSPtr	old_filter;

	old_filter = f_output_filter;
	f_output_filter = filter;

	return old_filter;
}



/************************************************************ DOC:

CLASS

	moOStream

NAME

	Flush - ensures all the data is written in the output

SYNOPSIS

	virtual int Flush(void);

DESCRIPTION

	The Flush() function will flush the data written to a disk, pipe,
	socket, etc. Any device which may bufferize data will derive the
	Flush() function so the user can ensure all the data was properly
	written in the output stream.

	The default Flush() function does nothing.

RETURN VALUE

	0 when no error occured
	non zero if an error occurs

SEE ALSO

	Write(), Put()

*/
int moOStream::Flush(void)
{
	if(f_output_filter != 0) {
		f_output_filter->Flush();
	}

	return 0;
}




/************************************************************ DOC:

CLASS

	moOStream

NAME

	OutputSize - the current size in bytes

SYNOPSIS

	virtual size_t OutputSize(void);

DESCRIPTION

	The OutputSize() function retrieves the current size of this
	output stream.

RETURN VALUE

	the size in bytes
	-1 if an error occur

SEE ALSO

	Write(), Put()

*/
size_t moOStream::OutputSize(void) const
{
	return 0;
}




/************************************************************ DOC:

CLASS

	moOStream

NAME

	WritePosition - get and set the position where data will be written next

SYNOPSIS

	virtual size_t WritePosition(void) const;
	virtual size_t WritePosition(size_t position);

DESCRIPTION

	The WritePosition() functions will be used to seek the output file
	to the position where data needs to be written.

RETURN VALUE

	the position where the pointer was before this call

SEE ALSO

	Write(), Put()

*/
size_t moOStream::WritePosition(void) const
{
	return f_output_position;
}


size_t moOStream::WritePosition(size_t position)
{
	size_t		old_position;

	old_position = f_output_position;
	f_output_position = static_cast<uint32_t>(position);

	return old_position;
}



/************************************************************ DOC:

CLASS

	moIOStream

NAME

	Constructor - create an I/O stream
	Destructor - destroys an I/O stream

SYNOPSIS

	moIOStream(void);
	virtual ~moIOStream();

DESCRIPTION

	Create a stream which can be used for input and output.

SEE ALSO

	moIStream, moOStream

*/
moIOStream::moIOStream(void)
{
}



moIOStream::~moIOStream()
{
}





/************************************************************ DOC:

CLASS

	moIOStream

NAME

	Filename - get or change the I/O stream filename

SYNOPSIS

	virtual void Filename(const char *filename);
	virtual const char *Filename(void);

PARAMETER

	filename - a UTF-8 filename

DESCRIPTION

	A stream, by itself, doesn't require a name. If it is a named
	file, however, it can be given a name using this function.

	Note that an I/O stream can receive two file names if the
	input and output specific functions are used to set the
	names.

RETURNS

	the current input and output stream filename

SEE ALSO

	moIStream::InputFilename(), moOStream::OutputFilename()

*/
void moIOStream::Filename(const char *filename)
{
	InputFilename(filename);
	OutputFilename(filename);
}



const char *moIOStream::Filename(void) const
{
	return InputFilename();
}



#if 0
/************************************************************ DOC:

CLASS

	moIOStream

NAME

	AddRef - remove the moBase ambiguity
	Release - remove the moBase ambiguity

SYNOPSIS

	unsigned long AddRef(void) const;
	unsigned long Release(void);

DESCRIPTION

	These two functions were created so the compiler can
	properly chose which AddRef/Release to use (the one
	from moIStream or moOStream.)

RETURN VALUE

	the number of times the object was referenced
 
SEE ALSO

	moBase::AddRef, moBase::Release

*/
unsigned long moIOStream::AddRef(void) const
{
	return moIStream::AddRef();
}

unsigned long moIOStream::Release(void) const
{
	return moIStream::Release();
}
#endif


};			// namespace molib;

// vim: ts=8 sw=8

