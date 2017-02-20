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





#ifndef MO_CRYPT_H
#define	MO_CRYPT_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_LIST_H
#include	"mo_list.h"
#endif
#ifndef MO_FILE_H
#include	"mo_file.h"
#endif
#ifndef MO_RANDOM_H
#include	"mo_random.h"
#endif


namespace molib
{



class MO_DLL_EXPORT moCryptBase : public moBase
{
public:
				moCryptBase(void);
	virtual			~moCryptBase();

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size) = 0;
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) = 0;
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) = 0;
	virtual void		Reset(void) = 0;
	virtual void		CheckTableSize(unsigned long& size);

	void			GenerateTable(moRandomBase *random, unsigned long size);
	void			SetTable(const unsigned char *data, unsigned long size);

	void			LoadTable(moFile& input);
	void			SaveTable(moFile& output);

	void			SetRandom(moRandomBase *random);

protected:
	virtual void		NewTable(void);		// call whenever a new table is installed
	virtual void		GenerateTable(void);	// overload in each moCryptBase

	void			RandomTable(void);	// generate tables of random 8 bits

	unsigned char *		f_table;	// table used to encode/decode
	unsigned long		f_table_size;	// number of bytes in f_table

	moRandomBaseSPtr	f_random;	// used to get pseudo-random values
};

typedef moSmartPtr<moCryptBase>		moCryptBaseSPtr;








/************************************************************ DOC:

CLASS

	moCryptBase

NAME

	SetRandom - defines a random object for this crypting object

SYNOPSIS

	void SetRandom(moRandomBase *random);

PARAMETERS

	random - the pseudo-random object used for this crypting object

DESCRIPTION

	This function can be used to define a pseudo-random object to
	be used by a crypting object.

	This random object can be used by multiple crypting object as
	long as these are not used by multiple threads simultaneously.

	You may reseed the random object at any time, yet, remember that
	to decode you may need to know precisely when to do so in order
	to get the proper results.

	Crypting objects which need to have a random generator defined
	specify that in their Encode() definition.

	You can use a null pointer (0) in order to detach a random object
	from a crypting object. It is important to do so before to
	destroy a random object if the crypting object is not to be
	destroyed too.

SEE ALSO

	Encode, Decode

*/
inline void moCryptBase::SetRandom(moRandomBase *random)
{
	f_random = random;
}





class MO_DLL_EXPORT moCrypt : public moList
{
public:
				moCrypt(void);

	void			Secure(void);

	long			DecodeSize(const unsigned char *src, unsigned long src_size) const;
	void			Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) const;
	void			Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size) const;
	void			Reset(void) const;

	void			LoadTables(moFile& input);
	void			SaveTables(moFile& output) const;

private:
	bool			f_secure;
};


/*
 * Quick list of encryptions available below.
 * The objects which can be used without a table are marked with an asterisk (*).
 * Some objects also don't require a table at all. In this case, it is marked
 * with a plus sign (+).
 * Objects which require or can handle a random object are marked with a hyphen
 * (-).
 *
 *	Add<size> (*)		add (encode) or subtract (decode) the table values to (from)
 *	(8, 16)			the source buffer on a per byte, short or long basis (<size>);
 *				without a table, add the current offset; the size has to be
 *				the proper multiple of bytes depending on the <size> parameter
 *
 *	XOr (*)			apply an xor operation of the table values on the source
 *				buffer on a per byte basis; the encoding and decoding applies
 *				the same operation; without a table, xor with the current
 *				offset;
 *
 *	Ror<size> (*)		apply a rotation of the value (a shift without loss of data);
 *	(8, 16, 32)		the table specifies the number of bits the byte, short or long
 *				is rotated (<size>); the size of the table must be a multiple
 *				of <size>;
 *				without a table, the offset is used as the number of bits to
 *				rotate
 *
 *	Mix (*)			move bytes at different offsets; the table specify the
 *				destination of the source in encode mode; the decoding
 *				requires the opposite table which will automatically be
 *				generated by this object; the offsets within the table
 *				are of type 'unsigned long', though the size is specified
 *				in bytes;
 *				to create a table for this encoder/decoder, create a table
 *				of increasing offsets (0, 1, 2 ... <size> - 1) and then
 *				swap the offsets randomly
 *
 *	Compress (+)		compressed the source (encoding); decompresses the source
 *				(decoding); the table is used as one long word on
 *				compression; that long word is taken as the level of
 *				compression; the compression/decompression use the z
 *				library functions of the same name
 *
 *	Code (-)		encode each byte as any of many possible 16 bit values;
 *				the table specify the decoding which is a 64Kb table of
 *				values going from 0 to 255; doing the following restore
 *				a value from an encoded buffer to the destination
 *				(decoded result):
 *
 *					dst[index] = code[src[index]];
 *
 *				where src is a pointer to unsigned shorts values and code/dst
 *				are pointers to unsigned chars (of 8 bits); to generate
 *				such a table one can (1) randomly generate the number of
 *				time each value will appear; (2) append that value to
 *				a table of 64Kb at the current position; (3) randomly
 *				mix the resulting table by generating two indices and
 *				swapping the values at these positions - this should be
 *				repeated 65536 or more times in order to ensure a good
 *				mixing
 *
 *				the encoding tables will automatically be generated within
 *				the object;
 *
 *				encode a buffer with this crypting object will multiple
 *				the size of the buffer by two and decode a crypted buffer
 *				will yield the normal size back;
 *
 *	Swap (*)		swap bytes from their value to another; this is similar to
 *				the Code operation except it doesn't double the bytes plus
 *				each byte is still encoded as one other byte (it's still
 *				fairly easy to decode such a buffer since all the letters
 *				A will be replaced with the same letter, for instance Z);
 *				The table must be exactly 256 bytes
 *
 *	Noise (*) (-)		enlarge the source buffer and add some noise between bytes
 *				(and even bits - a side effect if you use some Ror16/Ror32
 *				crypt objects before and/or after); this noise is totally
 *				random values which are generated with the most random
 *				function we can have on the computer; on encoding the output
 *				is larger than the source; on decoding, the noise is removed
 *				(totally ignored) and the output is smaller than the
 *				source; the table defines how much, where and how the noise
 *				is inserted
 *
 *	Size (+,*)		size the buffer; the primary use of this feature is to size
 *				the input buffer within itself; this is useful in case you
 *				are using the crypting feature by itself and entries such
 *				as the moCryptCompress are used
 *
 *	RSA			the data is transformed using the OpenSSL library; the table
 *				is a standard SSL key
 *
 *				NOT DONE YET!
 */


class MO_DLL_EXPORT moCryptAdd8 : public moCryptBase
{
public:
				moCryptAdd8(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};


class MO_DLL_EXPORT moCryptAdd16 : public moCryptBase
{
public:
				moCryptAdd16(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);
	virtual void		NewTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};



class MO_DLL_EXPORT moCryptXOr : public moCryptBase
{
public:
				moCryptXOr(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};


class MO_DLL_EXPORT moCryptRor8 : public moCryptBase
{
public:
				moCryptRor8(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};


class MO_DLL_EXPORT moCryptRor16 : public moCryptBase
{
public:
				moCryptRor16(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};


class MO_DLL_EXPORT moCryptRor32 : public moCryptBase
{
public:
				moCryptRor32(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_decode_index;
};


class MO_DLL_EXPORT moCryptMix : public moCryptBase
{
public:
				moCryptMix(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);
	virtual void		CheckTableSize(unsigned long& size);

protected:
	virtual void		GenerateTable(void);
	virtual void		NewTable(void);

private:
};



#define	MO_CRYPT_COMPRESS_VERSION		1000

struct MO_DLL_EXPORT moCryptCompressInfo {
	long			version;	// MO_CRYPT_COMPRESS_VERSION
	int			level;		// compression level
	unsigned long		input_size;	// if 0 - save input size in compressed buffer
};

class MO_DLL_EXPORT moCryptCompress : public moCryptBase
{
public:
				moCryptCompress(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);
	virtual void		CheckTableSize(unsigned long& size);

protected:
	virtual void		NewTable(void);

private:
	moCryptCompressInfo	f_info;
};



class MO_DLL_EXPORT moCryptSwap : public moCryptBase
{
public:
				moCryptSwap(void);
	virtual			~moCryptSwap();

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);
	virtual void		CheckTableSize(unsigned long& size);

protected:
	virtual void		NewTable(void);
	virtual void		GenerateTable(void);

private:
	unsigned char *		f_decode;	// table used in the Decode() function
};



class MO_DLL_EXPORT moCryptNoise : public moCryptBase
{
public:
				moCryptNoise(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);

protected:
	virtual void		NewTable(void);
	virtual void		GenerateTable(void);

private:
	unsigned long		f_encode_index;
	unsigned long		f_encode_copy;
	unsigned long		f_decode_index;
	unsigned long		f_decode_copy;
	bool			f_decode_skip;
};



class MO_DLL_EXPORT moCryptCode : public moCryptBase
{
public:
				moCryptCode(void);
	virtual			~moCryptCode();

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);
	virtual void		CheckTableSize(unsigned long& size);

protected:
	virtual void		NewTable(void);
	virtual void		GenerateTable(void);

private:
	unsigned short *	f_encode_count;		// # of times each value is repeated in the buffer
	unsigned short *	f_encode_index;		// count so each code is used one after another
	unsigned short **	f_encode_array;		// pointers pointing into f_encode_code directly to the proper values
	unsigned short *	f_encode_code;		// code to use to encode the next input value
};




class MO_DLL_EXPORT moCryptSize : public moCryptBase
{
public:
				moCryptSize(void);

	virtual long		DecodeSize(const unsigned char *src, unsigned long src_size);
	virtual void		Encode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Decode(const unsigned char *src, unsigned long src_size, unsigned char *& dst, unsigned long& dst_size);
	virtual void		Reset(void);
	virtual void		CheckTableSize(unsigned long& size);

private:
};






};			// namespace molib;

// vim: ts=8 sw=8
#endif	// #ifndef MO_CRYPT_H

