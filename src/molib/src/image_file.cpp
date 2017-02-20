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




// The implementation is in image.c++
//#ifdef MO_PRAGMA_INTERFACE
//#pragma implementation "mo/mo_image.h"
//#endif

#include	"mo/mo_image.h"



namespace molib
{





/************************************************************ DOC:

CLASS

	moImageFile

NAME

	Constructor - initialize and register this image file format
	Destructor - remove this image file format from the system list

SYNOPSIS

	moImageFile(void);
	virtual ~moImageFile();

DESCRIPTION

	The constructor of this object will ensure that the object is
	part of the list of file formats. This list is accessed each
	time an image file needs to be loaded or saved.

	The list is sorted by MimeType(). If two objects with the
	same mime type are inserted in the list, then an error is
	thrown.

	The destructor removes the object from the list since it can't
	be accessed anymore. This function is rather slow but it
	shouldn't be used except when people quit your application.

ERRORS

	A throw occurs if two objects with the same mime type are
	created.

BUGS

	The mime_type parameter to the constructor must be a constant
	string or a string which will leave for the time this object
	lives (i.e. the string is not copied in the object).

SEE ALSO

	Load, Save, MatchType, MimeType

*/
moImageFile::moImageFile(void)
{
	//g_new += *this; -- we can't do that internally... hmmm...
}


moImageFile::~moImageFile()
{
	// TODO: search this object in each list and remove it
}



/************************************************************ DOC:

CLASS

	moImageFile

NAME

	MimeType - returns the image file corresponding mime type

SYNOPSIS

	const char *MimeType(void) const;

DESCRIPTION

	The MimeType() function returns the mime type.

BUGS

	The returned string has to live at least for the time the
	object exists. Callers will expect it to be that way.

SEE ALSO

	Load, Save, MatchType

*/
/************************************************************ DOC:

CLASS

	moImageFile

NAME

	MatchPriority - priority over other image files formats
	MatchType - checks whether the input buffer matches this
		image type

SYNOPSIS

	virtual long MatchPriority(void) const = 0;
	virtual long MatchType(const void *buffer, unsigned long size) const = 0;

PARAMETERS

	buffer - a file buffer header
	size - the number of valid bytes in the buffer

DESCRIPTION

	The MatchType() function tests the input buffer within the
	constrained size for an image of the moImageFile object type.
	The function shall return a value from 0 to 100 (a percent).
	If 100 is returned, the fille is taken to be of that type.
	If a smaller value is returned, then the following image types
	are checked for a better percent value. The first object of
	which this function returns 100 or the object which returned
	the largest percent is selected as being the one which can
	load the specified image file.

	Because some image formats can very well be identified, they
	have a much higher priority. Other file formats, those for
	which we can't be 100% sure the MatchType() will return a
	correct answer, will have a smaller priority.

	In general, a standard type (recognized at 50%) will return
	a priority of 0 (such as TIFF which saves its header at the
	end of the file). A type which we are sure of will return
	a priority of +100 (such as GIF, PNG, JPEG). A type we are
	not sure about at all will return -100 (such as Targa).

	The priority can also be used to sort the file types in
	such an order that most popular formats are tested first
	so the search for the proper format is faster.

BUGS

	Some image types don't save a magic number at the start and will
	therefore be detected as (1) not being another type or (2) having
	all their header being valid (proper sizes and parameters in
	general).

SEE ALSO

	Load, Save, MimeType

*/
/************************************************************ DOC:

CLASS

	moImageFile

NAME

	Load - load an image from a file
	Save - save an image to a file

SYNOPSIS

	virtual bool Load(moIStream& file, moImage& im) = 0;
	virtual bool Load(moIStream& file, moImage16& im);
	virtual bool Load(moIStream& file, moImageFloat& im);
	virtual bool Save(moOStream& file, const moImage& im) = 0;
	virtual bool Save(moOStream& file, const moImage16& im);
	virtual bool Save(moOStream& file, const moImageFloat& im);

PARAMETERS

	file - a file where the image is read or written
	im - the image loaded or to be saved

DESCRIPTION

	The Load() function will be used to load an image
	from the input stream.
	
	When this function is called, the format of the input
	is already known since this function is only available
	on specific moImageFile objects. If you need to load
	images of different formats, you can let the system
	chosen which object to use to load your image. This
	is done with the static function moLoadImage().

	The Save() function creates an image file. The format
	is already known since this function is only available
	on specific moImageFile objects. If you want to save
	to different image formats without having to decide
	which moImageFile object to create, use the moSaveImage()
	function and specify a MIME type instead.

	The versions using an moImage16 format will be used to
	load images which support 16 bits. If a format doesn't
	support 16 bits then its 8 bits version will be used
	to load the image. Yet, a 16 bit image will be
	returned in the result. Similarly, it is possible to
	directly load an image of type moImageFloat.

	The Load() function will possibly set some parameters
	in the image object to diverse values found in the
	image file. These are described on a per format basis.
	For instance, if a comment is found in the file, it is
	put as the parameter "COMMENT" in the list of parameters
	of the image object.

	For some loaders it is also possible to affect the
	behavior of the loader by setting some parameters in
	the image object before to call the Load() function.
	Watch out! Such parameters may affect different loaders
	in a slightly different manner. It is wise at times
	to get a direct connection to a loader and use that
	instead of trying to wonder what is going to happen.

	The Save() function will possibly use some parameters
	already set in the image object. It will never change
	or add any parameters. The values will be used to
	save the images in different sub-formats. For instance,
	a DEPTH of GREYSCALE will force the image to be saved
	as a greyscale image.

	For the Save() function there are helper functions
	which will test some parameters which are common to
	different formats in order to know what they are set
	too. For instance, the DEPTH parameter can be set to
	GREY or GRAYSCALE and you will get the same effect.
	This specific parameter is tested in the DefineDepth()
	function.

SEE ALSO

	DefineDepth, MatchType, MatchPosition, MimeType

*/
bool moImageFile::Load(moIStream& file, moImage16& im)
{
	// when this function is called there is no 16 bits loader
	// so call the 8 bits version
	moImage		im8;

	if(Load(file, im8)) {
		im = im8;	// transform the 8 bit format in 16
		return true;
	}

	return false;
}


bool moImageFile::Save(moOStream& file, const moImage16& im)
{
	// when this function is called there is no 16 bits saver
	// so call the 8 bits version
	const moImage	im8(im);	// transform the 16 bit format in 8

	return Save(file, im8);
}



bool moImageFile::Load(moIStream& file, moImageFloat& im)
{
	// when this function is called there is no float loader
	// so call the 16 bits version
	moImage		im16;

	// TODO: if possible, we should avoid the convertion from
	//	 8 bits to 16...
	if(Load(file, im16)) {
		im = im16;	// transform the 16 bits format in float
		return true;
	}

	return false;
}


bool moImageFile::Save(moOStream& file, const moImageFloat& im)
{
	// when this function is called there is no Float bits saver
	// so call the 16 bits version
	const moImage	im16(im);	// transform the float format in 16 bits

	// TODO: if possible, we should avoid the convertion from
	//	 16 bits to 8...
	return Save(file, im16);
}



/************************************************************ DOC:

CLASS

	moImageFile

NAME

	DefineDepth - transform a depth name in a depth type

SYNOPSIS

	static mo_image_file_depth_t DefineDepth(const moImage& im, bool test_solid = false);

PARAMETERS

	im - the image which depth parameter is checked out
	test_solid - test whether the image is solid if no depth is defined

DESCRIPTION

	The Save() functions will usually check out the DEPTH parameter
	of the image they are about to save. In order to do that in
	a rather consistant way, this function was created. It will
	transform the following named depth in the corresponding
	mo_image_file_depth_t type.

	Type					Names

	MO_IMAGE_FILE_DEPTH_UNKNOWN		none of the following

	MO_IMAGE_FILE_DEPTH_BLACKNWHITE		"BLACKNWHITE"
						"B&W"
						"BNW"
						"BW"

	MO_IMAGE_FILE_DEPTH_PALETTED		"PALETTED"
						"PALETTE"

	MO_IMAGE_FILE_DEPTH_GREYSCALE		"GREY"
						"GREYSCALE"
						"GRAY"
						"GRAYSCALE"
						"Y"
						"LUMINANCE"

	MO_IMAGE_FILE_DEPTH_RED			"RED"
						"R"

	MO_IMAGE_FILE_DEPTH_GREEN		"GREEN"
						"G"

	MO_IMAGE_FILE_DEPTH_BLUE		"BLUE"
						"B"

	MO_IMAGE_FILE_DEPTH_ALPHA		"ALPHA"
						"A"
						"TRANSPARENCY"

	MO_IMAGE_FILE_DEPTH_GREYA		"GREYA"
						"GREYALPHA"
						"GREYSCALEA"
						"GREYSCALEALPHA"
						"GRAYA"
						"GRAYALPHA"
						"GRAYSCALEA"
						"GRAYSCALEALPHA"

	MO_IMAGE_FILE_DEPTH_RGB555		"RGB555"

	MO_IMAGE_FILE_DEPTH_RGB5551		"RGB5551"

	MO_IMAGE_FILE_DEPTH_RGB			"RGB"
						"COLOR"
						"COLOUR"

	MO_IMAGE_FILE_DEPTH_RGBA		"RGBA"
						"COLORA"
						"COLORALPHA"
						"COLOURA"
						"COLOURALPHA"

SEE ALSO

	DefineCompression, Save

*/
moImageFile::mo_image_file_depth_t moImageFile::DefineDepth(const moImage& im, bool test_solid)
{
	struct name2type {
		mo_image_file_depth_t	f_type;
		const char *		f_name;
	};

	static const name2type	all[] = {
		{ MO_IMAGE_FILE_DEPTH_BLACKNWHITE,	"BLACKNWHITE"		},
		{ MO_IMAGE_FILE_DEPTH_BLACKNWHITE,	"B&W"			},
		{ MO_IMAGE_FILE_DEPTH_BLACKNWHITE,	"BNW"			},
		{ MO_IMAGE_FILE_DEPTH_BLACKNWHITE,	"BW"			},

		{ MO_IMAGE_FILE_DEPTH_PALETTED,		"PALETTED"		},
		{ MO_IMAGE_FILE_DEPTH_PALETTED,		"PALETTE"		},

		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"GREY"			},
		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"GREYSCALE"		},
		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"GRAY"			},
		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"GRAYSCALE"		},
		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"Y"			},
		{ MO_IMAGE_FILE_DEPTH_GREYSCALE,	"LUMINANCE"		},

		{ MO_IMAGE_FILE_DEPTH_RED,		"RED"			},
		{ MO_IMAGE_FILE_DEPTH_RED,		"R"			},
		{ MO_IMAGE_FILE_DEPTH_GREEN,		"GREEN"			},
		{ MO_IMAGE_FILE_DEPTH_GREEN,		"G"			},
		{ MO_IMAGE_FILE_DEPTH_BLUE,		"BLUE"			},
		{ MO_IMAGE_FILE_DEPTH_BLUE,		"B"			},
		{ MO_IMAGE_FILE_DEPTH_ALPHA,		"ALPHA"			},
		{ MO_IMAGE_FILE_DEPTH_ALPHA,		"TRANSPARENCY"		},

		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GREYA"			},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GREYALPHA"		},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GREYSCALEA"		},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GREYSCALEALPHA"	},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GRAYA"			},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GRAYALPHA"		},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GRAYSCALEA"		},
		{ MO_IMAGE_FILE_DEPTH_GREYA,		"GRAYSCALEALPHA"	},

		{ MO_IMAGE_FILE_DEPTH_RGB555,		"RGB555"		},

		{ MO_IMAGE_FILE_DEPTH_RGB5551,		"RGB5551"		},

		{ MO_IMAGE_FILE_DEPTH_RGB,		"RGB"			},
		{ MO_IMAGE_FILE_DEPTH_RGB,		"COLOR"			},
		{ MO_IMAGE_FILE_DEPTH_RGB,		"COLOUR"		},

		{ MO_IMAGE_FILE_DEPTH_RGBA,		"RGBA"			},
		{ MO_IMAGE_FILE_DEPTH_RGBA,		"COLORA"		},
		{ MO_IMAGE_FILE_DEPTH_RGBA,		"COLOURA"		},
		{ MO_IMAGE_FILE_DEPTH_RGBA,		"COLORALPHA"		},
		{ MO_IMAGE_FILE_DEPTH_RGBA,		"COLOURALPHA"		},

		{ MO_IMAGE_FILE_DEPTH_RGBX,		"RGBX"			},
		{ MO_IMAGE_FILE_DEPTH_RGBX,		"RGBPAD"		},
		{ MO_IMAGE_FILE_DEPTH_RGBX,		"RGB32"			},

		// EOT
		{ MO_IMAGE_FILE_DEPTH_UNKNOWN,		0			}
	};

	mo_image_file_depth_t	result;
	const name2type		*l;
	char			c;

	moWCString *depth = im.Parameters().Get("DEPTH");

	/* depth may not be defined, if it is, look for the value within our list of all depths */
	if(depth != 0) {
		c = depth->Get(0);
		l = all;
		while(l->f_type != MO_IMAGE_FILE_DEPTH_UNKNOWN) {
			if(l->f_name[0] == c && *depth == l->f_name) {
				break;
			}
			l++;
		}
		result = l->f_type;
	}
	else {
		result = MO_IMAGE_FILE_DEPTH_UNKNOWN;
	}

	/* last chance... is it a solid image or not? */
	if(result == MO_IMAGE_FILE_DEPTH_UNKNOWN && test_solid) {
		result = im.IsSolid() ? MO_IMAGE_FILE_DEPTH_RGB : MO_IMAGE_FILE_DEPTH_RGBA;
	}

	return result;
}



/************************************************************ DOC:

CLASS

	moImageFile

NAME

	DefineCompression - transform a compression name in a compression type

SYNOPSIS

	static mo_image_file_compression_t DefineCompression(const moImage& im);

PARAMETERS

	im - the image which depth parameter is checked out

DESCRIPTION

	The Save() functions will usually check out the COMPRESSION
	parameter of the image they are about to save. In order to do
	that in a rather consistant way, this function was created.
	It will transform the following named compressions in the
	corresponding mo_image_file_compression_t type.

	In most cases, if only one compression is supported, the
	default compression (or best) will be used if the specified
	compression isn't understood. Only if you get a NONE will
	you not compressed (unless there is no compression support
	in the given format like PPM).

	The special TEST entry will be used by formats which support
	multiple compression schemes to find the best one for the
	given image. This can take a long time since it will compress
	the image using each compression scheme then compare which
	one's best and keep that one.

	Type					Names

	MO_IMAGE_FILE_COMPRESSION_UNKNOWN	none of the following

	MO_IMAGE_FILE_COMPRESSION_NONE		"NONE"
						"VERBATIM"

	MO_IMAGE_FILE_COMPRESSION_BEST		"BEST"

	MO_IMAGE_FILE_COMPRESSION_TEST		"TEST"

	MO_IMAGE_FILE_COMPRESSION_RLE		"RLE"

SEE ALSO

	DefineDepth, Save

*/
moImageFile::mo_image_file_compression_t moImageFile::DefineCompression(const moImage& im)
{
	struct name2type {
		mo_image_file_compression_t	f_type;
		const char *			f_name;
	};

	static const name2type	all[] = {
		{ MO_IMAGE_FILE_COMPRESSION_NONE,	"NONE"			},
		{ MO_IMAGE_FILE_COMPRESSION_NONE,	"VERBATIM"		},

		{ MO_IMAGE_FILE_COMPRESSION_BEST,	"BEST"			},

		{ MO_IMAGE_FILE_COMPRESSION_TEST,	"TEST"			},

		{ MO_IMAGE_FILE_COMPRESSION_RLE,	"RLE"			},

		// EOT
		{ MO_IMAGE_FILE_COMPRESSION_UNKNOWN,	0			}
	};

	mo_image_file_compression_t	result;
	const name2type			*l;
	char				c;

	moWCString *compression = im.Parameters().Get("COMPRESSION");

	/* depth may not be defined, if it is, look for the value within our list of all depths */
	if(compression != 0) {
		c = compression->Get(0);
		l = all;
		while(l->f_type != MO_IMAGE_FILE_COMPRESSION_UNKNOWN) {
			if(l->f_name[0] == c && *compression == l->f_name) {
				break;
			}
			l++;
		}
		result = l->f_type;
	}
	else {
		result = MO_IMAGE_FILE_COMPRESSION_UNKNOWN;
	}

	return result;
}



/************************************************************ DOC:

CLASS

	moImageFile

NAME

	Compare - compare two image file mime types
	ComparePriority - compare two image file match priority

SYNOPSIS

	compare_t Compare(const moBase& object) const;
	compare_t ComparePriority(const moBase& object) const;

PARAMETERS

	object - another image file

DESCRIPTION

	The Compare() function is used to sort the image file
	formats by their MIME types.

	The ComparePriority() function is used to sort the
	image file formats by priority. This sorted list is
	used to quickly find a proper match of a file format
	from a file header.

SEE ALSO

	Load, Save, MimeType, MatchType, MatchPriority,
	moFindImageFile

*/
moBase::compare_t moImageFile::Compare(const moBase& object) const
{
	int		r;

	r = strcmp(MimeType(), dynamic_cast<const moImageFile&>(object).MimeType());

	return r < 0 ? MO_BASE_COMPARE_SMALLER :
		(r == 0 ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_GREATER);
}


moBase::compare_t moImageFile::ComparePriority(const moBase& object) const
{
	int		r;

	r = MatchPriority() - dynamic_cast<const moImageFile&>(object).MatchPriority();

	return r < 0 ? MO_BASE_COMPARE_SMALLER :
		(r == 0 ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_GREATER);
}




/************************************************************ DOC:

CLASS

	moImageFile

NAME

	moLoadImage - load an image from a file
	moSaveImage - save an image to a file

SYNOPSIS

	bool moLoadImage(moIStream& file, moImage& im);
	bool moLoadImage(moIStream& file, moImage16& im);
	bool moLoadImage(moIStream& file, moImageFloat& im);
	bool moSaveImage(moOStream& file, const moImage& im, const char *mime_type = 0);
	bool moSaveImage(moOStream& file, const moImage16& im, const char *mime_type = 0);
	bool moSaveImage(moOStream& file, const moImageFloat& im, const char *mime_type = 0);

PARAMETERS

	file - a file where the image is read or written
	im - the image loaded or to be saved
	mime_type - the image file format used to save the image in

DESCRIPTION

	The Load() function will be used to load any image
	format currently supported by the library or loaded
	as plug ins.

	The Save() function creates a file of the specified
	image file format from the given image.

	It is possible to set the FILEFORMAT parameter in the image
	instead of using the mime_type parameter. Note that the
	loaders will set that parameter automatically. Thus, it
	is possible to load an image in a given format and by
	default it will be saved back in that same format.

	The version with 16 bits images will load/save 8 bits images
	if the specified format doesn't allow for 16 bits. Then
	the image will automatically be transformed in a 16 bits
	image and returned to the caller.

SEE ALSO

	Load, Save, MimeType, MatchType, MatchPriority,
	moFindImageFile

*/
bool moLoadImage(moIStream& file, moImage& im)
{
	char		buffer[1024];			/* Flawfinder: ignore */
	int		size;
	size_t		pos;
	moImageFile	*imfile;

	pos = file.ReadPosition();
	size = file.Read(buffer, sizeof(buffer));
	if(size <= 0) {
		// errors and empty files are rejected
#if defined(MO_DEBUG) && 0
fprintf(stderr, "moLoadImage(); failed to load the file?! (%d)\n", size);
#endif
		return false;
	}
	// go back to where we were
	file.ReadPosition(pos);

#if defined(MO_DEBUG) && 0
fprintf(stderr, "moLoadImage(\"%s\", im);\n", file.InputFilename());
#endif

	imfile = moFindImageFile(buffer, static_cast<unsigned long>(size));
	if(imfile == 0) {
#if defined(MO_DEBUG) && 0
fprintf(stderr, "moLoadImage(); failed to find the mime type for this image\n");
#endif
		return false;
	}

	im.Parameters().Set("FILEFORMAT", imfile->MimeType());

	return imfile->Load(file, im);
}


bool moLoadImage(moIStream& file, moImage16& im)
{
	char		buffer[1024];		/* Flawfinder: ignore */
	int		size;
	size_t		pos;
	moImageFile	*imfile;

	pos = file.ReadPosition();
	size = file.Read(buffer, sizeof(buffer));
	if(size <= 0) {
		// errors and empty files are rejected
		return false;
	}
	// go back to where we were
	file.ReadPosition(pos);

	imfile = moFindImageFile(buffer, static_cast<unsigned long>(size));
	if(imfile == 0) {
		return false;
	}

	im.Parameters().Set("FILEFORMAT", imfile->MimeType());

	return imfile->Load(file, im);
}


bool moLoadImage(moIStream& file, moImageFloat& im)
{
	char		buffer[1024];	/* Flawfinder: ignore */
	int		size;
	size_t		pos;
	moImageFile	*imfile;

	pos = file.ReadPosition();
	size = file.Read(buffer, sizeof(buffer));
	if(size <= 0) {
		// errors and empty files are rejected
		return false;
	}
	// go back to where we were
	file.ReadPosition(pos);

	imfile = moFindImageFile(buffer, static_cast<unsigned long>(size));
	if(imfile == 0) {
		return false;
	}

	im.Parameters().Set("FILEFORMAT", imfile->MimeType());

	return imfile->Load(file, im);
}


bool moSaveImage(moOStream& file, const moImage& im, const char *mime_type)
{
	moImageFile		*imfile;
	moWCString		*file_format;
	bool			result;

	if(mime_type == 0) {
		file_format = im.Parameters().Get("FILEFORMAT");
		if(file_format != 0) {
			mime_type = file_format->SavedMBData();
		}
	}
	imfile = moFindImageFile(mime_type);
	if(imfile != 0) {
		result = imfile->Save(file, im);
	}
	else {
		result = false;
	}

	return result;
}


bool moSaveImage(moOStream& file, const moImage16& im, const char *mime_type)
{
	moImageFile	*imfile;
	moWCString	*file_format;
	bool		result;

	if(mime_type == 0) {
		file_format = im.Parameters().Get("FILEFORMAT");
		if(file_format != 0) {
			mime_type = file_format->SavedMBData();
		}
	}
	imfile = moFindImageFile(mime_type);
	if(imfile == 0) {
		result = false;
	}
	else {
		result = imfile->Save(file, im);
	}

	return result;
}


bool moSaveImage(moOStream& file, const moImageFloat& im, const char *mime_type)
{
	moImageFile	*imfile;
	moWCString	*file_format;
	bool		result;

	if(mime_type == 0) {
		file_format = im.Parameters().Get("FILEFORMAT");
		if(file_format != 0) {
			mime_type = file_format->SavedMBData();
		}
	}
	imfile = moFindImageFile(mime_type);
	if(imfile == 0) {
		result = false;
	}
	else {
		result = imfile->Save(file, im);
	}

	return result;
}



/************************************************************ DOC:

CLASS

	moImageFileFactory

NAME

	RegisterFormat - call to register a new image format

SYNOPSIS

	void RegisterFormat(moImageFile *format);

	static moImageFileFactorySPtr GetFactory(void);
	static void Done(void);

	private:
	void InitLists(void);

PARAMETERS

	format - format to register

DESCRIPTION

	The moRegisterFormat() functions is used by each image file
	format supported to register themselves to the factory. The
	factory is in turn used to access to the different instances
	of each format.

	The GetFactory() function will be used to access the image
	file factory. Note that there can be only one factory and
	for this reason, the function will automatically create one
	factory on the first call, and then always return the same
	factory over and over again.

	In DEBUG mode, you may want to track the leaks and thus you
	can call Done() to clean up all the memory used by this
	class and all the registered classes.

	Note that the GetFactory() function call will generate an
	assert after a call to the Done() function.

	The GetMIMEList() function returns a list of image formats
	sorted by MIME

	The very first time you try to access the list of available
	image formats, you get a call to the InitLists() which
	creates the f_mime_list and f_match_list.

RETURN VALUE

	The GetFactory() function returns a smart pointer to the
	moImageFileFactory for your process.

BUGS

	The very first GetFactory() needs to be done before you create
	another thread which may also use it.

SEE ALSO


*/
moImageFileFactory *	moImageFileFactory::g_factory;
bool			moImageFileFactory::g_done = false;



moImageFileFactory::moImageFileFactory(void)
{
	f_registers = 0;
}


moImageFileFactory::~moImageFileFactory()
{
}


const char *moImageFileFactory::moGetClassName(void) const
{
	return "moBase::moImageFileFactory";
}


moImageFileFactorySPtr moImageFileFactory::GetFactory(void)
{
	if(!g_factory) {
		assert(!g_done);
		g_factory = new moImageFileFactory;
		g_factory->AddRef();
	}

	return g_factory;
}

void moImageFileFactory::Done(void)
{
	if(!g_done) {
		g_done = true;
		g_factory->Release();
		g_factory = 0;
	}
}


void moImageFileFactory::Register(moImageFileRegister& format)
{
	format.SetNext(f_registers);
	f_registers = &format;
}


void moImageFileFactory::InitLists(void) const
{
	moImageFileRegister	*r;

	// we need to have this done multi-thread safe
	moLockMutex lock(f_mutex);

	// already done?
	if(f_mime_list != 0) {
		return;
	}

	moImageFileSPtr	imfile;

	f_mime_list = new moSortedListUniqueOfImageFile;

	f_match_list = new moSortedListOfImageFile;
	f_match_list->SetCompare(reinterpret_cast<moBase::compare_function_t>(&moImageFile::ComparePriority));

	r = f_registers;
	while(r != 0) {
		imfile = r->Create();
		if(!f_mime_list->Insert(imfile)) {
			throw moError(MO_ERROR_INVALID, "two moImageFile_<type> use the same MIME type: \"%s\"", imfile->MimeType());
		}
		*f_match_list += *imfile;
		r = r->GetNext();
	}

#if defined(MO_DEBUG) && 0
fprintf(stderr, "moImageFileFactory: we have %ld mime formats & %ld match formats.\n",
				f_mime_list->Count(), f_match_list->Count());
for(unsigned long idx = 0; idx < f_mime_list->Count(); idx++) {
	imfile = f_mime_list->Get(idx);
	fprintf(stderr, "  . %s (%ld)\n", imfile->MimeType(), imfile->MatchPriority());
}
#endif


// The following was the old way. Neat with a static lib, but doesn't work with
// a shared lib. It's too bad we can't have our own text and data sections all
// the time... 8-(
#if 0
	// the start and stop entries are generated by the GNU linker
	extern mo_imagefile_func_t	__start__mo_imagefile_tab[];
	extern mo_imagefile_func_t	__stop__mo_imagefile_tab[];
	long				idx;
	moImageFile			*imfile;
	mo_imagefile_func_t		*func;
	static moSortedListUnique	*g_mime_list;
	static moSortedList		*g_match_list;

	// TODO: this function needs to be multi-thread protected

	if(g_mime_list == 0) {
		g_mime_list = new moSortedListUnique;
		g_match_list = new moSortedList;
		g_match_list->SetCompare(reinterpret_cast<moBase::compare_function_t>(&moImageFile::ComparePriority));
		idx = __stop__mo_imagefile_tab - __start__mo_imagefile_tab;

#ifdef MO_DEBUG
printf("Insert %ld image file format types.\n", idx);
#endif

		func = __start__mo_imagefile_tab;
		while(idx > 0) {
			idx--;
			imfile = (**func)();
			if(imfile == 0) {
				throw std::bad_alloc();
			}
			if(!g_mime_list->Insert(imfile)) {
				throw moError(MO_ERROR_INVALID, "two moImageFile_<type> use the same MIME type");
			}
			*g_match_list += *imfile;
			func++;
		}

#ifdef MO_DEBUG
printf("We now have %ld & %ld entries.\n", g_mime_list->Count(), g_match_list->Count());
for(idx = 0; idx < (long) g_mime_list->Count(); idx++) {
	imfile = (moImageFile *) g_mime_list->Get(idx);
	printf("  . %s (%ld)\n", imfile->MimeType(), imfile->MatchPriority());
}
#endif

	}

	return which == 0 ? g_mime_list : g_match_list;
#endif
}


moSortedListUniqueOfImageFileSPtr moImageFileFactory::GetMIMEList(void) const
{
	InitLists();
	return f_mime_list;
}


moSortedListOfImageFileSPtr moImageFileFactory::GetMatchList(void) const
{
	InitLists();
	return f_match_list;
}



/************************************************************ DOC:

CLASS

	moImageFile

NAME

	moFindImageFile - look for an image file format

SYNOPSIS

	moImageFile *moFindImageFile(const char *mime_type);
	moImageFile *moFindImageFile(const void *buffer, unsigned long size);

PARAMETERS

	buffer - size of the buffer to match for a type
	size - the size of the buffer in bytes
	mime_type - the mime type of the image file format seeked

DESCRIPTION

	The moFindImageFile() functions will be used to search for
	an image file format among all the image file formats
	currently registered in the system which match the specified
	input parameter(s).

	When a MIME is specified, the function compares each known
	image file format MIME type. Only one image file format can
	accept a given MIME type.

	When a buffer and a size is specified, the function searches
	for a matching format. All the formats will always be tested
	unless one returns that it matched 100%.

BUGS

	At this time these functions are not safe in a multi-thread
	environment.

SEE ALSO

	MimeType, MatchType, MatchPriority,
	moLoadImage, moSaveImage

*/
class moSearchImageFile : public moImageFile
{
public:
				moSearchImageFile(const char *mime_type)
				{
					f_mime_type = mime_type;
				}

	virtual const char *	MimeType(void) const
				{
					return f_mime_type;
				}

	virtual long		MatchPriority(void) const
				{
					// unused
					return 0;
				}

	virtual long		MatchType(const void *buffer, unsigned long size) const
				{
					// unused
					return 0;
				}

	virtual bool		Load(moIStream& file, moImage& im)
				{
					// unused
					return false;
				}

	virtual bool		Save(moOStream& file, const moImage& im)
				{
					// unused
					return false;
				}
private:
	const char *		f_mime_type;
};


moImageFileSPtr moFindImageFile(const char *mime_type)
{
	moList::position_t			p;
	moSortedListUniqueOfImageFileSPtr	lst;

	// make sure lists are properly initialized
	lst = moImageFileFactory::GetFactory()->GetMIMEList();

	moSearchImageFile find(mime_type);

	p = lst->Find(&find);
	if(p == moList::NO_POSITION) {
		return static_cast<moImageFile *>(0);
	}

	return lst->Get(p);
}



moImageFileSPtr moFindImageFile(const void *buffer, unsigned long size)
{
	moList::position_t		p;
	moImageFileSPtr			imfile, best_imfile;
	long				v, max;
	moSortedListOfImageFileSPtr	lst;

	// make sure lists are properly initialized
	lst = moImageFileFactory::GetFactory()->GetMatchList();

	max = 0;
	p = lst->Count();
	while(p > 0UL) {
		p--;
		imfile = lst->Get(p);
		v = imfile->MatchType(buffer, size);

#if defined(MO_DEBUG) && 0
printf("moFindImageFile: tested #%ld [%s] with a result of %ld\n",
			(unsigned long) p, imfile->MimeType(), v);
#endif

		if(v >= 100) {
			return imfile;
		}
		if(v > max) {
			best_imfile = imfile;
			max = v;
		}
	}

	return best_imfile;
}



};			// namespace molib;

// vim: ts=8

