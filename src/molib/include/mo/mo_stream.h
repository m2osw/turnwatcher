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



#ifndef MO_STREAM_H
#define	MO_STREAM_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_FIFO_H
#include	"mo_fifo.h"
#endif
#ifndef MO_TEMPLATE_H
#include	"mo_template.h"
#endif

namespace molib
{

class MO_DLL_EXPORT moIStream : public virtual moBase
{
public:
	static const long	MAX_UNGET_SIZE = 16;

				moIStream(void);
	virtual			~moIStream();

	virtual void		InputFilename(const char *filename);
	virtual const char *	InputFilename(void) const;

	virtual int		SetInputEndianess(int endian);
	virtual moFIFOSPtr	SetInputFilter(moFIFO *filter);

	virtual int		Get(bool& c);
	virtual int		Get(signed char& c);
	virtual int		Get(unsigned char& c);
	virtual int		Get(wchar_t& c);
	virtual int		Get(int16_t& c);
	virtual int		Get(uint16_t& c);
	virtual int		Get(int32_t& c);
	virtual int		Get(uint32_t& c);
#if __WORDSIZE == 32
	virtual int		Get(signed long& c);
	virtual int		Get(unsigned long& c);
#endif
	virtual int		Get(int64_t& c);
	virtual int		Get(uint64_t& c);
	virtual int		Get(float& c);
	virtual int		Get(double& c);
#ifndef MO_CONFIG_NO_LONG_DOUBLE
	virtual int		Get(long double& c);
#endif

	virtual int		Unget(bool c);
	virtual int		Unget(signed char c);
	virtual int		Unget(unsigned char c);
	virtual int		Unget(wchar_t c);
	virtual int		Unget(int16_t c);
	virtual int		Unget(uint16_t c);
	virtual int		Unget(int32_t c);
	virtual int		Unget(uint32_t c);
#if __WORDSIZE == 32
	virtual int		Unget(signed long c);
	virtual int		Unget(unsigned long c);
#endif
	virtual int		Unget(int64_t c);
	virtual int		Unget(uint64_t c);
	virtual int		Unget(float c);
	virtual int		Unget(double c);
#ifndef MO_CONFIG_NO_LONG_DOUBLE
	virtual int		Unget(long double c);
#endif

	virtual	size_t		ReadPosition(void) const;
	virtual	size_t		ReadPosition(size_t new_pos);
	virtual int		Read(void *buffer, size_t length);
	virtual int		Unread(const void *buffer, size_t length);

	virtual size_t		InputSize(void) const;

	void			Secret(void);

protected:
	virtual int		RawRead(void *buffer, size_t length) = 0;

	mint32_t		f_input_endian;
	zsize_t			f_input_position;
	moFIFOSPtr		f_input_filter;
	char *			f_input_filename;
	zsize_t			f_input_unget_position;
	unsigned char		f_input_unget[MAX_UNGET_SIZE];

private:
				// no copy
				moIStream(const moIStream& stream)
					: moBase(stream),
					  f_input_endian(stream.f_input_endian)
					{}
	moIStream&		operator = (const moIStream& stream) { return *this; }

	zbool_t			f_secret;
};

typedef moSmartPtr<moIStream>	moIStreamSPtr;


class MO_DLL_EXPORT moIStreamScopeFilter
{
public:
				moIStreamScopeFilter(moIStream& stream, moFIFO *filter)
					: f_released(false),
					  f_stream(stream),
					  f_old_filter(f_stream.SetInputFilter(filter))
				{
				}

				~moIStreamScopeFilter()
				{
					Release();
				}

	void			Release()
				{
					if(!f_released) {
						f_released = true;
						f_stream.SetInputFilter(f_old_filter);
					}
				}

private:
	bool			f_released;
	moIStream&		f_stream;
	moFIFOSPtr		f_old_filter;
};



class MO_DLL_EXPORT moOStream : public virtual moBase
{
public:
				moOStream(void);
	virtual			~moOStream();

	virtual void		OutputFilename(const char *filename);
	virtual const char *	OutputFilename(void) const;

	virtual int		SetOutputEndianess(int endian);
	virtual moFIFOSPtr	SetOutputFilter(moFIFO *filter);

	virtual int		Put(bool c);
	virtual int		Put(signed char c);
	virtual int		Put(unsigned char c);
	virtual int		Put(wchar_t c);
	virtual int		Put(int16_t c);
	virtual int		Put(uint16_t c);
	virtual int		Put(int32_t c);
	virtual int		Put(uint32_t c);
#if __WORDSIZE == 32
	virtual int		Put(signed long c);
	virtual int		Put(unsigned long c);
#endif
	virtual int		Put(int64_t c);
	virtual int		Put(uint64_t c);
	virtual int		Put(float c);
	virtual int		Put(double c);
#ifndef MO_CONFIG_NO_LONG_DOUBLE
	virtual int		Put(long double c);
#endif

	virtual	size_t		WritePosition(void) const;
	virtual	size_t		WritePosition(size_t new_pos);
	virtual int		Write(const void *buffer, size_t length);
	virtual int		Flush(void);

	virtual size_t		OutputSize(void) const;

protected:
	virtual int		RawWrite(const void *buffer, size_t length) = 0;

	mint32_t		f_output_endian;
	zsize_t			f_output_position;
	moFIFOSPtr		f_output_filter;
	char *			f_output_filename;

private:
				// no copy
				moOStream(const moOStream& stream)
					: moBase(stream),
					  f_output_endian(stream.f_output_endian)
					{}
	moOStream&		operator = (const moOStream& stream) { return *this; }
};

typedef moSmartPtr<moOStream>	moOStreamSPtr;


class MO_DLL_EXPORT moOStreamScopeFilter
{
public:
				moOStreamScopeFilter(moOStream& stream, moFIFO *filter)
					: f_released(false),
					  f_stream(stream),
					  f_old_filter(f_stream.SetOutputFilter(filter))
				{
				}

				~moOStreamScopeFilter()
				{
					Release();
				}

	void			Release()
				{
					if(!f_released) {
						f_released = true;
						f_stream.SetOutputFilter(f_old_filter);
					}
				}

private:
	bool			f_released;
	moOStream&		f_stream;
	moFIFOSPtr		f_old_filter;
};



class MO_DLL_EXPORT moIOStream : public moIStream, public moOStream
{
public:
				moIOStream(void);
	virtual			~moIOStream();

	virtual	void		Filename(const char *filename);
	virtual const char *	Filename(void) const;

				//operator const moBase& (void) const { return static_cast<const moIStream&>(*this); }
				//operator moBase& (void) { return static_cast<moIStream&>(*this); }
				//operator const moBase * (void) const { return static_cast<const moIStream *>(this); }
				//operator moBase * (void) { return static_cast<moIStream *>(this); }
	//unsigned long		AddRef(void) const;
	//unsigned long		Release(void);
};

typedef moDualSmartPtr<moIOStream, moIStream>	moIOStreamSPtr;







};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_STREAM_H

