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
#pragma implementation "mo/mo_image.h"
#endif

#include	"mo/mo_image.h"

namespace molib
{



/* the following comes from the JPEG library
 * which declares this array to transform the
 * targa 5 bits in good 8 bits pixels. This
 * is the best and fastest way to get a good
 * precision.
 */
const unsigned char mo5BitsTo8Bits[32] = {	/* Flawfinder: ignore */
    0,   8,  16,  25,  33,  41,  49,  58,
   66,  74,  82,  90,  99, 107, 115, 123,
  132, 140, 148, 156, 165, 173, 181, 189,
  197, 206, 214, 222, 230, 239, 247, 255,
};



/************************************************************ DOC:

CLASS

	moImageBase

NAME

	private:
	Init - initialize an image structure

SYNOPSIS

	void Init(void);

DESCRIPTION

	This function is used internally so all constructors can
	initialize the image object the same way.

	The base init is called by the moImageBase constructor,
	which will be called before the moImage, moImage16 and
	moImageFloat contructors.

SEE ALSO

	Constructor

*/
void moImageBase::Init(void)
{
	f_width     = 0;
	f_width_p2  = 0;
	f_height    = 0;
	f_height_p2 = 0;
	f_depth     = 0;	// the specilized image formats initialize this one later
	f_size      = 0;
	f_max       = 0;
	f_errno     = 0;
	f_data      = 0;
}









/************************************************************ DOC:

CLASS

	moImageBase
	moImage
	moImage16
	moImageFloat

NAME

	Constructor - initialize or copy an image
	Destructor - release all the resources used by an image

	operator = - copy an image (with format convertion when required)

SYNOPSIS

	moImage(void);
	moImage16(void);
	moImageFloat(void);
	moImage(const moImage& im);
	moImage(const moImage16& im);
	moImage(const moImageFloat& im);
	moImage16(const moImage& im);
	moImage16(const moImage16& im);
	moImage16(const moImageFloat& im);
	moImageFloat(const moImage& im);
	moImageFloat(const moImage16& im);
	moImageFloat(const moImageFloat& im);
	moImage(unsigned long width, unsigned long height);
	moImage(unsigned long width, unsigned long height, const moRGBA& clear);
	moImage16(unsigned long width, unsigned long height);
	moImage16(unsigned long width, unsigned long height, const moRGBA16& clear);
	moImageFloat(unsigned long width, unsigned long height);
	moImageFloat(unsigned long width, unsigned long height, const moRGBAFloat& clear);
	virtual ~moImageBase();

	moImage& operator = (const moImage& im);
	moImage& operator = (const moImage16& im);
	moImage& operator = (const moImageFloat& im);
	moImage16& operator = (const moImage& im);
	moImage16& operator = (const moImage16& im);
	moImage16& operator = (const moImageFloat& im);
	moImageFloat& operator = (const moImage& im);
	moImageFloat& operator = (const moImage16& im);
	moImageFloat& operator = (const moImageFloat& im);

PARAMETERS

	im - an image to duplicate
	width - the new image width
	height - the new image height
	clear - the color used to clear the new image

DESCRIPTION

	The moImage constructors will be used to create new images or
	copy an existing image in a new image object. The function used
	without parameters creates an image holder. There won't be any
	data and the image sizes will be (0, 0).

	The function which accepts an image as a parameter will duplicate
	that image in the new object. The function accepts sizes and
	possibly a color (clear) will be used to create an image with
	these sizes and possible clears it with the specified color
	(by default an image is not cleared).

	The destructor ensures all the resources used by the image are
	released back to the system.

NOTE

	It is possible to create an empty image in which case no resource
	are allocated. If either the width or height is zero, then the
	other is set to zero also.

SEE ALSO

	Init

*/
moImageBase::moImageBase(void)
	: f_parameters("image")
{
	Init();
}


moImage::moImage(void)
{
	f_depth = sizeof(moRGBA);
}


moImage16::moImage16(void)
{
	f_depth = sizeof(moRGBA16);
}


moImageFloat::moImageFloat(void)
{
	f_depth = sizeof(moRGBAFloat);
}




template<class T>
void CopyImageData(T *d, const T *s, unsigned long size)
{
	if(d == 0 || s == 0) {
		return;
	}

	// in this case we don't need a convertion
	// so a simple memcpy() works!
	memcpy(d, s, size * sizeof(T));	/* Flawfinder: ignore */
}


template<class D, class S>
void CopyImageData(D *d, const S *s, unsigned long size)
{
	if(d == 0 || s == 0) {
		return;
	}

	// the following applies the convertions between
	// the type S and D
	while(size > 0) {
		size--;
		*d++ = *s++;
	}
}



moImage::moImage(const moImage& im)
	: moImageBase()
{
	f_depth = sizeof(moRGBA);
	SetSize(im.f_width, im.f_height);
	CopyImageData(Data(), im.Data(), f_size);
}


moImage16::moImage16(const moImage16& im)
	: moImageBase()
{
	f_depth = sizeof(moRGBA16);
	SetSize(im.f_width, im.f_height);
	CopyImageData(Data(), im.Data(), f_size);
}


moImageFloat::moImageFloat(const moImageFloat& im)
	: moImageBase()
{
	f_depth = sizeof(moRGBAFloat);
	SetSize(im.f_width, im.f_height);
	CopyImageData(Data(), im.Data(), f_size);
}


moImage::moImage(const moImage16& im)
{
	f_depth = sizeof(moRGBA);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}




moImageFloat::moImageFloat(const moImage16& im)
{
	f_depth = sizeof(moRGBAFloat);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}


moImage16::moImage16(const moImage& im)
{
	f_depth = sizeof(moRGBA16);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}


moImageFloat::moImageFloat(const moImage& im)
{
	f_depth = sizeof(moRGBAFloat);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}


moImage::moImage(const moImageFloat& im)
{
	f_depth = sizeof(moRGBA);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}


moImage16::moImage16(const moImageFloat& im)
{
	f_depth = sizeof(moRGBA16);
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);
}


moImage::moImage(unsigned long width, unsigned long height)
{
	f_depth = sizeof(moRGBA);
	SetSize(width, height);
}


moImage16::moImage16(unsigned long width, unsigned long height)
{
	f_depth = sizeof(moRGBA16);
	SetSize(width, height);
}


moImageFloat::moImageFloat(unsigned long width, unsigned long height)
{
	f_depth = sizeof(moRGBAFloat);
	SetSize(width, height);
}


moImage::moImage(unsigned long width, unsigned long height, const moRGBA& clear)
{
	f_depth = sizeof(moRGBA);
	SetSize(width, height);
	Clear(clear, 0, 0, width, height);
}


moImage16::moImage16(unsigned long width, unsigned long height, const moRGBA16& clear)
{
	f_depth = sizeof(moRGBA16);
	SetSize(width, height);
	Clear(clear, 0, 0, width, height);
}


moImageFloat::moImageFloat(unsigned long width, unsigned long height, const moRGBAFloat& clear)
{
	f_depth = sizeof(moRGBAFloat);
	SetSize(width, height);
	Clear(clear, 0, 0, width, height);
}


moImageBase::~moImageBase()
{
	delete f_data;
}


moImage::~moImage()
{
}


moImage16::~moImage16()
{
}


moImageFloat::~moImageFloat()
{
}




moImage& moImage::operator = (const moImage& im)
{
	if(this != &im) {
		SetSize(im.f_width, im.f_height);
		CopyImageData(Data(), im.Data(), f_size);
	}

	return *this;
}


moImage16& moImage16::operator = (const moImage16& im)
{
	if(this != &im) {
		SetSize(im.f_width, im.f_height);
		CopyImageData(Data(), im.Data(), f_size);
	}

	return *this;
}


moImageFloat& moImageFloat::operator = (const moImageFloat& im)
{
	if(this != &im) {
		SetSize(im.f_width, im.f_height);
		CopyImageData(Data(), im.Data(), f_size);
	}

	return *this;
}


moImage& moImage::operator = (const moImage16& im)
{
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);

	return *this;
}


moImageFloat& moImageFloat::operator = (const moImage16& im)
{
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);

	return *this;
}


moImage16& moImage16::operator = (const moImage& im)
{
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);

	return *this;
}


moImageFloat& moImageFloat::operator = (const moImage& im)
{
	SetSize(im.Width(), im.Height());
	CopyImageData(Data(), im.Data(), f_size);

	return *this;
}






/************************************************************ DOC:

CLASS

	moImageBase
	moImage
	moImage16
	moImageFloat

NAME

	SetSize - set/change the size of the image

SYNOPSIS

	void SetSize(unsigned long width, unsigned long height, bool safe = false);

	protected:
	void NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height);

PARAMETERS

	data - a pointer to the image data
	width - the new image width
	height - the new image height
	safe - make it as safe as possible for the data

DESCRIPTION

	The SetSize() function will be used to change the sizes of an
	image. This function doesn't resize the image data, it only
	sets the size of the buffer to the given width and height.

	When the safe flag is set to true the function keeps as much
	data as it possibly can in the process (i.e. the top left of
	the image remains the same). Note that setting the safe flag
	to true and elarging an image makes the new area undefined.

NOTES

	A safer resizing of the image buffer will be archived with the
	use of the Crop() or Enlarge() functions instead. To resize
	the image data also, use the Resize() function.

SEE ALSO

	Constructors, Crop

*/
void moImageBase::SetSize(unsigned long width, unsigned long height, bool safe)
{
	unsigned char	*data;

	// make sure empty images are empty both ways
	if(width == 0) {
		height = 0;
	}
	else if(height == 0) {
		width = 0;
	}

	// does it change?
	if(width == f_width && height == f_height) {
		return;
	}

	if(width > MAX_WIDTH || height > MAX_HEIGHT) {
		// invalid sizes, ignore them!
		assert(0);	// in debug, tell the programmer about it
		errno = f_errno = EINVAL;
		return;
	}

	// NOTE: we don't need to test the heights since both
	//	 the width and the height will be zero if the
	//	 image is empty
	f_size = width * height;
	if(safe && f_width != 0 && width != 0) {
		data = new unsigned char[f_size * Depth()];
		NewSizeCopy(data, width, height);
		delete f_data;
		f_data = data;
		f_max = f_size;
	}
	else if(f_size > f_max) {
		delete f_data;
		f_data = 0;
		f_data = new unsigned char[f_size * Depth()];
		f_max = f_size;
	}
	f_width = width;
	f_height = height;

	f_width_p2 = 0;
	f_height_p2 = 0;
}





void moImage::NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height)
{
	CopyImageData(reinterpret_cast<moRGBA *>(data), width, height, Data(), f_width, f_height, 0, 0, 0, 0, width, height);
}


void moImage16::NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height)
{
	CopyImageData(reinterpret_cast<moRGBA16 *>(data), width, height, Data(), f_width, f_height, 0, 0, 0, 0, width, height);
}


void moImageFloat::NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height)
{
	CopyImageData(reinterpret_cast<moRGBAFloat *>(data), width, height, Data(), f_width, f_height, 0, 0, 0, 0, width, height);
}






/************************************************************ DOC:

CLASS

	moRGBA
	moRGBA16
	moRGBAFloat

NAME

	CopyImageData - copy an area into another

SYNOPSIS

	template<class D, class S>
	static void CopyImageData(D *destination, unsigned long dw, unsigned long dh,
			const S *source, unsigned long sw, unsigned long sh,
			long x, long y, long sx, long sy,
			unsigned long width, unsigned long height);

PARAMETERS

	destination - the destination buffer
	dw - width of the destination buffer
	dh - height of the destination buffer
	source - the source buffer
	sw - width of the source buffer
	sh - height of the source buffer
	x - position where to copy into the destination
	y - position where to copy into the destination
	sx - position where to copy from in the source
	sy - position where to copy from in the source
	width - the width to copy
	height - the height to copy

DESCRIPTION

	The CopyImageData() function duplicates the pixels defined by
	the (sx, sy, width, height) rectangle to the (x, y, width,
	height) rectangle in the destination.

	The function takes care of clipping the rectangle as required
	to ensure that the copy doesn't read or write data outside the
	source and destination buffers. It is possible that the clipping
	results in no copy being performed at all.

	Since it is a template, the source and destination can both be
	of any pixel type. The necessary convertions will automatically
	be applied.

SEE ALSO

	ClearImage

*/





/************************************************************ DOC:

CLASS

	moImage
	moImage16
	moImageFloat

NAME

	OpenGLTexCompatible - ensure that the texture is compatible with OpenGL textures

SYNOPSIS

	void OpenGLTexCompatible(void);

DESCRIPTION

	The OpenGLTexCompatible() function ensures that the image has a height which is
	an exact power of 2. Note that OpenGL textures do NOT need to have a width which
	is a multiple of 2 because you can specify the width of the image with a
	glPixelStore() call:

		glPixelStorei(GL_UNPACK_ROW_LENGTH, texture_width);

	Note that you still have to specify the width as a power of two to the texture
	loading function:

		glTexImage2D(GL_TEXTURE_2D, 0, 4, my_image.WidthP2(), my_image.HeightP2(),
						0, GL_RGBA, GL_UNSIGNED_BYTE, my_image.Data());

	Once you are done, it is strongly suggested that you reset the pixel store value
	to the default:

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	which cancels the feature.

NOTES

	This function uses SetSize() and tells it that it is safe to copy the image
	data as is. This has the effect to enlarge the image if required. Note however
	that the part which is added to the image in this way will be garbage.

SEE ALSO

	CopyImage, Resize, SetSize

*/
void moImage::OpenGLTexCompatible(void)
{
	SetSize(f_width, HeightP2(), true);
}



void moImage16::OpenGLTexCompatible(void)
{
	SetSize(f_width, HeightP2(), true);
}



void moImageFloat::OpenGLTexCompatible(void)
{
	SetSize(f_width, HeightP2(), true);
}





/************************************************************ DOC:

CLASS

	moImage
	moImage16
	moImageFloat

NAME

	Clear - clear the image with the given color

SYNOPSIS

	void Clear(const moRGBA& clear, long x, long y,
			unsigned long width, unsigned long height);
	void Clear(const moRGBA16& clear, long x, long y,
			unsigned long width, unsigned long height);
	void Clear(const moRGBAFloat& clear, long x, long y,
			unsigned long width, unsigned long height);

	template<class D, class C>
	void ClearImage(D *d, unsigned long dw, unsigned long dh,
			const C& clear, long x, long y,
			unsigned long width, unsigned long height);

PARAMETERS

	d - pointer to the image data
	dw - width of the image data buffer
	dh - height of the image data buffer
	clear - the color to use to clear the image
	x - the first pixel to clear horizontally
	y - the first pixel to clear vertically
	width - the number of pixels to clear horizontally
	height - the number of pixels to clear vertically

DESCRIPTION

	The Clear() function can be used to copy a color in a rectangle
	in an image. It is possible to use the ClearImage() template in
	order to use a different type for the image data and the color
	used to clear the image.

SEE ALSO

	CopyImageData

*/
void moImage::Clear(const moRGBA& clear, long x, long y, unsigned long width, unsigned long height)
{
	ClearImage(Data(), f_width, f_height, clear, x, y, width, height);
}

void moImage16::Clear(const moRGBA16& clear, long x, long y, unsigned long width, unsigned long height)
{
	ClearImage(Data(), f_width, f_height, clear, x, y, width, height);
}

void moImageFloat::Clear(const moRGBAFloat& clear, long x, long y, unsigned long width, unsigned long height)
{
	ClearImage(Data(), f_width, f_height, clear, x, y, width, height);
}






/************************************************************ DOC:

CLASS

	moImageBase
	moImage
	moImage16
	moImageFloat

NAME

	AlphaTest - test whether the entire image is a solid,
		transparent or a combinaison or if it is empty
	IsSolid - check whether all the pixels are non-transparent
	IsTransparent - check whether all the pixels are 100%
		transparent
	IsCombinaison - check whether any pixel is neither solid
		nor fully transparent

SYNOPSIS

	alpha_test_t AlphaTest(void) const;
	bool IsCombinaison(void) const;
	bool IsSolid(void) const;
	bool IsTransparent(void) const;

DESCRIPTION

	These functions check every pixel to know whether they
	all are solid or transparent. If a mix is found, the image
	is determined to be a COMBINAISON. If only solid pixels are
	found, the image is said to be SOLID. When all the pixels
	are transparent, the image is TRANSPARENT. If the image is
	empty, then this function returns EMPTY.

	The function checks the solidity and transparency of pixels
	using the pixel IsSolid() and IsTransparent() functions.

	An empty window is considered solid.

SEE ALSO

	IsEmpty

*/
template<class T>
moImageBase::alpha_test_t DoAlphaTest(const T *s, unsigned long size)
{
	if(s == 0 || size == 0) {
		return moImageBase::ALPHA_TEST_EMPTY;
	}

	if(s->IsTransparent()) {
		while(size > 0) {
			size--;
			if(!s->IsTransparent()) {
				return moImageBase::ALPHA_TEST_COMBINAISON;
			}
			s++;
		}
		return moImageBase::ALPHA_TEST_TRANSPARENT;
	}

	while(size > 0) {
		size--;
		if(!s->IsSolid()) {
			return moImageBase::ALPHA_TEST_COMBINAISON;
		}
		s++;
	}

	return moImageBase::ALPHA_TEST_SOLID;
}


moImageBase::alpha_test_t moImage::AlphaTest(void) const
{
	return DoAlphaTest(Data(), f_size);
}


moImageBase::alpha_test_t moImage16::AlphaTest(void) const
{
	return DoAlphaTest(Data(), f_size);
}


moImageBase::alpha_test_t moImageFloat::AlphaTest(void) const
{
	return DoAlphaTest(Data(), f_size);
}



bool moImageBase::IsTransparent(void) const
{
	return AlphaTest() == ALPHA_TEST_TRANSPARENT;
}


bool moImageBase::IsSolid(void) const
{
	alpha_test_t at = AlphaTest();
	return at == ALPHA_TEST_SOLID || at == ALPHA_TEST_EMPTY;
}


bool moImageBase::IsCombinaison(void) const
{
	return AlphaTest() == ALPHA_TEST_COMBINAISON;
}




/************************************************************ DOC:

CLASS

	moImageBase
	moImage
	moImage16
	moImageFloat

NAME

	ApplyAlpha - apply the alpha to the image color components

SYNOPSIS

	void ApplyAlpha(void);

DESCRIPTION

	The ApplyAlpha() function multiply all the color components with
	their respective alpha channel information. This way you can use
	the source image as is when blending it with another image; this
	often eliminates artifacts when the image isn't rendered at an
	exact pixel position with OpenGL.

	Whenever a source image has had its alpha channel applied, it
	needs to use the following blend mode in OpenGL:

		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Also, images saved in a Flash movies with an alpha channel need
	to have their color components premultiplied in this way.

SEE ALSO

	AlphaTest

*/
template<class T>
void DoApplyAlpha(T *s, unsigned long size)
{
	while(size > 0) {
		size--;
		s->ApplyAlpha();
		s++;
	}
}


void moImage::ApplyAlpha(void)
{
	return DoApplyAlpha(Data(), f_size);
}


void moImage16::ApplyAlpha(void)
{
	return DoApplyAlpha(Data(), f_size);
}


void moImageFloat::ApplyAlpha(void)
{
	return DoApplyAlpha(Data(), f_size);
}


/************************************************************ DOC:

CLASS

	moImage
	moImage16

NAME

	Histogram - count all the colors of the image

SYNOPSIS

	moArray *Histogram(void) const;

DESCRIPTION

	This function can be used to count the colors present in an
	image. This will be used by different functions such as the
	Palette() function which generates a palette for an image.

	It is the caller responsability to delete the array once
	it is not necessary any more.

NOTE

	An empty image has no color and returns an empty array.

	At this time there is no histogram for the floating point
	format since there can be a way too large number of color
	in such an image.

RETURN VALUE

	The function returns an array of moCountColor[16].

SEE ALSO

	Palette

*/
moBase::compare_t moCompareCountColor(const void *a, const void *b)
{
	const moCountColor *ca = reinterpret_cast<const moCountColor *>(a);
	const moCountColor *cb = reinterpret_cast<const moCountColor *>(b);

	if(ca->color.red < cb->color.red) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.red > cb->color.red) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.green < cb->color.green) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.green > cb->color.green) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.blue < cb->color.blue) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.blue > cb->color.blue) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.alpha < cb->color.alpha) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.alpha > cb->color.alpha) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}

	return moBase::MO_BASE_COMPARE_EQUAL;
}


moArrayBase *moImage::Histogram(void) const
{
	const moRGBA		*s;
	unsigned long		sz;
	moArrayBase		*array;
	moCountColor		count_color;
	moArray::position_t	p;

	array = new moSortedArrayUnique(sizeof(moCountColor), moCompareCountColor);
	array->SetStep(5000);		// quick growth accelerate the process slightly

	s = Data();
	sz = f_size;
	while(sz > 0) {
		sz--;
		count_color.color = *s;
		p = array->Find(&count_color);
		if(p == moArray::NO_POSITION) {
			// not inserted yet, create a new entry
			count_color.count = 1;
			array->Insert(&count_color);
		}
		else {
			reinterpret_cast<moCountColor *>(array->Get(p))->count++;
		}
		s++;
	}

	return array;
}




moBase::compare_t moCompareCountColor16(const void *a, const void *b)
{
	const moCountColor16 *ca = reinterpret_cast<const moCountColor16 *>(a);
	const moCountColor16 *cb = reinterpret_cast<const moCountColor16 *>(b);

	if(ca->color.red < cb->color.red) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.red > cb->color.red) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.green < cb->color.green) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.green > cb->color.green) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.blue < cb->color.blue) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.blue > cb->color.blue) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}
	if(ca->color.alpha < cb->color.alpha) {
		return moBase::MO_BASE_COMPARE_SMALLER;
	}
	if(ca->color.alpha > cb->color.alpha) {
		return moBase::MO_BASE_COMPARE_GREATER;
	}

	return moBase::MO_BASE_COMPARE_EQUAL;
}


moArrayBase *moImage16::Histogram(void) const
{
	const moRGBA16		*s;
	unsigned long		sz;
	moArrayBase		*array;
	moCountColor16		count_color;
	moArray::position_t	p;

	array = new moSortedArrayUnique(sizeof(moCountColor16), moCompareCountColor16);
	array->SetStep(5000);		// quick growth accelerate the process slightly

	count_color.count = 1;
	s = Data();
	sz = f_size;
	while(sz > 0) {
		sz--;
		count_color.color = *s;
		if(!array->Insert(&count_color)) {
			// already inserted, increase the counter
			p = array->Find(&count_color);
			reinterpret_cast<moCountColor16 *>(array->Get(p))->count++;
		}
		s++;
	}

	return array;
}







/************************************************************ DOC:

CLASS

	moImage

NAME

	Paletize - transform an image in a palette of colors
		and an array of indexes

SYNOPSIS

	void *Paletize8(unsigned long& count8) const;
	void *Paletize16(unsigned long& count16) const;

PARAMETERS

	count8 - the maximum number of colors to be returned
		(usually 256 - supports any value from 16 to 256)

	count16 - the maximum number of colors to be returned
		(supports any value from 16 to 65536)

	also the count8/count16 are set to the number of
	colors defined in the resulting palette

DESCRIPTION

	Whenever it is necessary to save an image in a file format
	which uses a palette and an array of indexes, one needs to
	call this function.

	It will first count the total number of colors in an image.
	If that total is smaller or equal to the maximumm specified
	by the caller, then a palette is built from the existing
	colors and there won't be any loss of color information.

	When there are too many colors, the function attempts to
	reduces them by grouping them. Groups are defined by
	transforming colors from the usual red, green, blue and
	alpha components to the most significant bits of each
	of these components to the least significant. By sorting
	the colors this way, groups are easilly generated. Clearing
	the four lower bits, then 8, 12, etc. we can merge the
	colors from different groups and generate a smaller number
	of colors.

NOTES

	The function won't work very well when used to generate
	palettes with a very small number of colors (under 64).

	It is possible to used a dithering to transform a color
	image in a black and white image.

	The source image is not modified by this function.

	There isn't such functions for the moImage16 since it
	doesn't seem necessary. You will have to convert you
	16 bit image in an 8 bit first.

RETURN VALUE

	0 if the function fails (empty image).

	Otherwise the function returns an array of indexes. The
	size of the indexes will be of 8 or 16 bits (an unsigned
	char * or unsigned short * pointer). No palette of more
	than 65536 colors can be created with this function.

SEE ALSO

	Palette

*/
#if 0
void moColorGroup::Set(const moCountColor& color_count)
{
	unsigned long	planes, idx;

	f_color_count = color_count;

	planes =  (color_count.color.red   << 24)
		| (color_count.color.green << 16)
		| (color_count.color.blue  <<  8)
		|  color_count.color.alpha;

	f_group = 0;
	idx = 8;
	do {
		f_group <<= 4;
		// we order the bits so the colors are sorted by
		// "importance" (luminance) - the alpha is
		// considered the most important component...
		f_group |= ((long)  planes        < 0 ? 2 : 0)
			 | ((long) (planes <<  8) < 0 ? 4 : 0)
			 | ((long) (planes << 16) < 0 ? 1 : 0)
			 | ((long) (planes << 24) < 0 ? 8 : 0);
		planes <<= 1;
		idx--;
	} while(idx > 0);

	f_count = 0;
}


moBase::compare_t moCompareColorGroup(const void *a, const void *b)
{
	const moColorGroup *&ca = reinterpret_cast<const moColorGroup *&>(a);
	const moColorGroup *&cb = reinterpret_cast<const moColorGroup *&>(b);

	return ca->f_group > cb->f_group ? moBase::MO_BASE_COMPARE_GREATER
		: (ca->f_group < cb->f_group ? moBase::MO_BASE_COMPARE_SMALLER
				: moBase::MO_BASE_COMPARE_EQUAL);
}


moBase::compare_t moCompareColorGroupCount(const void *a, const void *b)
{
	moColorGroup * const *&ca = reinterpret_cast<moColorGroup * const *&>(a);
	moColorGroup * const *&cb = reinterpret_cast<moColorGroup * const *&>(b);

	return ca[0]->f_count > cb[0]->f_count ? moBase::MO_BASE_COMPARE_GREATER
		: (ca[0]->f_count < cb[0]->f_count ? moBase::MO_BASE_COMPARE_SMALLER
				: moBase::MO_BASE_COMPARE_EQUAL);
}


unsigned char *moImage::Paletize8(unsigned long& count, moRGBA *& palette) const
{
	const moRGBA		*s;
	moRGBA			compare;
	moArrayBase		*array, *groups;
	unsigned long		cnt, sz, last, group_count;
	long			mask;
	unsigned char		*indexes, *d;
	moCountColor		count_color;
	moArray::position_t	p;
	moColorGroup		grp, *g, *gcnt;

// valid input?
	if(IsEmpty() || count < 16 || count > 256) {
		return 0;
	}

// create the resulting indexes & palette now
 	palette = new moRGBA[count];
	indexes = new unsigned char[f_size];

// count the total number of colors
	array = Histogram();
	cnt = array->Count();
	if(cnt <= count) {
		// there isn't more colors than what the user
		// asked for; so don't do any further quantization
		count = cnt;
		s = Data();
		sz = f_size;
		d = indexes;
		do {
			count_color.color = *s;
			p = array->Find(&count_color);
			// here p should always be defined in [0..count)
			if(p >= cnt) {
				// problem!
				throw moBug(MO_ERROR_INTERNAL, "pointer too large");
			}
			*d = p;
			d++;
			s++;
			sz--;
		} while(sz > 0);
		return indexes;
	}

// when there are too many colors we sort them by groups
// in order to reduce that number
	groups = new moSortedArray(sizeof(moColorGroup), moCompareColorGroup);
	groups->SetArraySize(cnt);

	do {
		cnt--;
		grp.Set(*reinterpret_cast<const moCountColor *>(array->Get(cnt)));
		groups->Insert(&grp);
	} while(cnt > 0);

	mask = 0xF0000000;
	while(true) {
		last = static_cast<unsigned long>(-1);
		cnt = groups->Count();
		sz = 0;
		do {
			cnt--;
			g = reinterpret_cast<moColorGroup *>(groups->Get(cnt));
			if((g->f_group & mask) != last) {
				last = g->f_group & mask;
				sz++;
			}
		} while(cnt > 0);

printf("With mask %08lX we got %ld entries\n", mask, sz);

		if(sz >= count) {
			break;
		}
		group_count = sz;
		mask >>= 4;
	}

	if(sz == count) {
		// here we have a special case (i.e. we don't need to change anything more!)
		// this is important in case 16 colors are required and faster in all the
		// other cases
		//group_count = sz;	// this is useless since this is also equal to count

		// We should have a special faster case, but doing the following is enough
		// for a working algorithm
		mask >>= 4;
	}
	mask <<= 4;

// create an array ordered by the number of colors in a given group
	moSortedArray counted_groups(sizeof(moColorGroup *), moCompareColorGroupCount);
	counted_groups.SetArraySize(group_count);

	last = static_cast<unsigned long>(-1);
	cnt = groups->Count();
	gcnt = 0;
	do {
		cnt--;
		g = reinterpret_cast<moColorGroup *>(groups->Get(cnt));
		if((g->f_group & mask) != last) {
			if(gcnt != 0) {
				// insert the previous group
				gcnt->f_count = sz;
				counted_groups.Insert(&gcnt);
			}
			last = g->f_group & mask;
			gcnt = g;
			sz = 1;
		}
		else {
			sz++;
		}
	} while(cnt > 0);
	gcnt->f_count = sz;
	counted_groups.Insert(&gcnt);

printf("Done... result is: %ld (%ld)\n", group_count, counted_groups.Count());
if(1) for(cnt = 0, group_count = 0; cnt < counted_groups.Count(); cnt++) {
g = *reinterpret_cast<moColorGroup **>(counted_groups.Get(cnt));
group_count += g->f_count;
printf("Group = 0x%08lX Count = %3ld (%ld)\n", g->f_group, g->f_count, group_count);
}

// now we can use the group with the most colors and try to
// cut it in smaller groups



// now transform the image in a palette + array of indexes
	last = static_cast<unsigned long>(-1);
	sz = static_cast<unsigned long>(-1);
	group_count = groups->Count();
	do {
		group_count--;
		g = reinterpret_cast<moColorGroup *>(groups->Get(group_count));
		if((g->f_group & mask) != last) {
			// use a different index
			last = g->f_group & mask;
			sz++;
		}
		// search the source image for this entry
		// (this is a really bad - but simple algorithm)
		compare = g->f_color_count.color;
		cnt = f_size;
		s = data() + cnt;
		do {
			cnt--;
			s--;
			if(compare == *s) {
				indexes[cnt] = sz;
			}
		} while(cnt > 0);
	} while(group_count > 0);


	return indexes;
}
#endif


void moColorLeaf::Set(const moRGBA& color)
{
	unsigned long	idx, planes;

	f_type   = 'L';
	f_locked = false;
	f_count  = 1;
	f_red    = color.red;
	f_green  = color.green;
	f_blue   = color.blue;
	f_alpha  = color.alpha;

	planes =  (color.red   << 24)
		| (color.green << 16)
		| (color.blue  <<  8)
		|  color.alpha;
	idx = 8;
	do {
		// we order the bits so the colors are sorted by
		// "importance" (luminance) - the alpha is
		// considered the most important component...
		f_group <<= 4;
		f_group |= ((long)  planes        < 0 ? 2 : 0)
			 | ((long) (planes <<  8) < 0 ? 4 : 0)
			 | ((long) (planes << 16) < 0 ? 1 : 0)
			 | ((long) (planes << 24) < 0 ? 8 : 0);
		planes <<= 1;
		idx--;
	} while(idx > 0);
}


moRGBA moColorLeaf::Get(void) const
{
	moRGBA		result;

	result.red   = static_cast<unsigned char>((f_red   + f_count - 1) / f_count);
	result.green = static_cast<unsigned char>((f_green + f_count - 1) / f_count);
	result.blue  = static_cast<unsigned char>((f_blue  + f_count - 1) / f_count);
	result.alpha = static_cast<unsigned char>((f_alpha + f_count - 1) / f_count);

	return result;
}


void moColorBranch::Init(void)
{
	f_type     = 'B';
	f_parent   = 0;
	f_next     = 0;
	f_previous = 0;
	memset(f_leaves, 0, sizeof(f_leaves));
}


moColorInfo::moColorInfo(void)
{
	memset(f_level, 0, sizeof(f_level));
	memset(&f_root, 0, sizeof(f_root));
	f_branches    = 0;
	f_next_branch = 0;
	f_leaves      = 0;
	f_next_leaf   = 0;
	f_index       = 0;
	f_allocated   = 0;
}


moColorInfo::~moColorInfo()
{
	moColorBuffer	*b;

	while(f_branches != 0) {
		b = f_branches;
		f_branches = b->f_next;
		delete b;
	}
	while(f_leaves != 0) {
		b = f_leaves;
		f_leaves = b->f_next;
		delete b;
	}

	delete f_palette;
}


moColorBranch *moColorInfo::AllocBranch(void)
{
	moColorBuffer	*b;
	moColorBranch	*l;
	unsigned long	idx;

	// any more leaf available?
	if(f_next_branch == 0) {
		b = reinterpret_cast<moColorBuffer *>(new unsigned char[sizeof(moColorBuffer) + sizeof(moColorBranch) * 1000]);
		f_allocated += sizeof(moColorBuffer) + sizeof(moColorBranch) * 1000;
		// 1000 leaves were allocated, link them together
		l = reinterpret_cast<moColorBranch *>(b + 1);
		f_next_branch = l;
		idx = 999;
		do {
			* reinterpret_cast<moColorBranch **>(l) = l + 1;
			l++;
			idx--;
		} while(idx > 0);
		* reinterpret_cast<moColorBranch **>(l) = 0;
	}

	// return the next available leaf and unlink it
	l = f_next_branch;
	f_next_branch = * reinterpret_cast<moColorBranch **>(l);

	l->Init();

	return l;
}


void moColorInfo::FreeBranch(moColorBranch *branch)
{
	if(branch->f_next != 0) {
		branch->f_next->f_previous = branch->f_previous;
	}
	if(branch->f_previous != 0) {
		branch->f_previous->f_next = branch->f_next;
	}
	else {
		// this was the first branch, change the level
		f_level[(unsigned char) branch->f_level] = branch->f_next;
	}
	* reinterpret_cast<moColorBranch **>(branch) = f_next_branch;
	f_next_branch = branch;
}



moColorLeaf *moColorInfo::AllocLeaf(void)
{
	moColorBuffer	*b;
	moColorLeaf	*l;
	unsigned long	idx;

	// any more leaf available?
	if(f_next_leaf == 0) {
		b = reinterpret_cast<moColorBuffer *>(new unsigned char[sizeof(moColorBuffer) + sizeof(moColorLeaf) * 1000]);
		f_allocated += sizeof(moColorBuffer) + sizeof(moColorLeaf) * 1000;
		// 1000 leaves were allocated, link them together
		l = reinterpret_cast<moColorLeaf *>(b + 1);
		f_next_leaf = l;
		idx = 999;
		do {
			* reinterpret_cast<moColorLeaf **>(l) = l + 1;
			l++;
			idx--;
		} while(idx > 0);
		* reinterpret_cast<moColorLeaf **>(l) = 0;
	}

	// return the next available leaf and unlink it
	l = f_next_leaf;
	f_next_leaf = * reinterpret_cast<moColorLeaf **>(l);

	return l;
}



moColorLeaf *moColorInfo::Simplify(moColorBranch *b)
{
	unsigned long	idx;
	moColorLeaf	*l;

	for(idx = 0; idx < 16; idx++) {
		l = b->f_leaves[idx];
		if(l != 0) {
			if(l->f_type == 'B') {
				b = reinterpret_cast<moColorBranch *>(l);
				l = Simplify(b);
				FreeBranch(b);
				return l;
			}
			if(l->f_type == 'L') {
				l->f_locked = 1;
				return l;
			}
		}
	}
for(idx = 0; idx < 16; idx++) {
	l = b->f_leaves[idx];
	if(l != 0) {
		printf("Leaf = %p (%c)\n", l, l->f_type);
	}
}
	/* we should never reach this place */
	throw moError(MO_ERROR_INVALID, "can't find leave to simplify");
	/*NOTREACHED*/
}



unsigned long moColorInfo::Reduce(void)
{
	unsigned long	idx, i, j, k, delta, min, smaller, /*min_idx,*/ min_i, min_j;
	moColorBranch	*b, *min_b;
	moColorLeaf	*l, *r;

	min_i = 0;
	min_j = 0;
	min_b = 0;

	min = 1000;
	for(idx = 0; idx < 8; idx++) {
		b = f_level[idx];
		if(b == 0) {
			// once we found some branches, there are branches
			// all the way, however, we may have to go through
			// multiple levels before to find the first branch
			continue;
		}
		do {
			smaller = 1000;
			i = static_cast<unsigned long>(-1);
			j = static_cast<unsigned long>(-1);
			for(k = 0; k < 16; k++) {
				l = b->f_leaves[k];
				if(l == 0) {
					// no color
					continue;
				}
				if(l->f_type == '>') {
					// already linked
					continue;
				}
				i = j;
				j = k;
				delta = j - i;
				if(i < 8 && j >= 8) {
					delta = j - i + 16;
				}
				if(i != static_cast<unsigned long>(-1) && delta < smaller) {
					smaller = delta;
					if(smaller < min) {
						min = smaller;
						//min_idx = idx;
						min_i = i;
						min_j = j;
						min_b = b;
						if(min == 1) {
							// in this case we really
							// don't need to test anything
							// more! we found one of the
							// best case to merge
							goto merge;
						}
					}
				}
			}
			b = b->f_next;
		} while(b != 0);
		if(min == 1000) {
			// didn't find anything at this level, try next
			continue;
		}
merge:
		// we got something, let's work on it
		l = min_b->f_leaves[min_i];
		if(l->f_type == 'B') {	// a branch needs simplification
			b = reinterpret_cast<moColorBranch *>(l);
			l = Simplify(b);
			FreeBranch(b);
			min_b->f_leaves[min_i] = l;
		}
		r = min_b->f_leaves[min_j];
		if(r->f_type == 'B') {	// a branch needs simplification
			b = reinterpret_cast<moColorBranch *>(r);
			r = Simplify(b);
			FreeBranch(b);
			min_b->f_leaves[min_j] = r;
		}
		l->f_count += r->f_count;
		l->f_red   += r->f_red;
		l->f_green += r->f_green;
		l->f_blue  += r->f_blue;
		l->f_alpha += r->f_alpha;
		r->f_type = '>';	// this is now a linked node
		r->f_extra = l;
		return r->f_index;	// now free index
	}
	/* arriving here means there is nothing to reduce?!? */
	throw moError(MO_ERROR_INTERNAL, "can't reduce colors without any leaf");
	/*NOTREACHED*/
}


moColorLeaf *moColorInfo::Insert(const moColorLeaf& leave)
{
	moColorLeaf	*l;
	moColorBranch	*b, *p;
	unsigned long	level, idx, color;

	level = 32;
	b = &f_root;
	do {
		level -= 4;
		idx = (leave.f_group >> level) & 15;
		l = b->f_leaves[idx];
		if(l == 0) {
			// We MUST link the new leaf BEFORE we reduce otherwise the
			// Reduce() could throw the branch 'b' away!
			l = AllocLeaf();
			b->f_leaves[idx] = l;
			*l = leave;
			l->f_index = f_index;
#if 0
// NOTE: shouldn't we reduce only at the end?
			if(f_index >= f_max) {
				// Ha! We already got too many indexes - need to reduce
				color = Reduce();
			}
			else {
				// get the next available index
				color = f_index;
				f_index++;
			}
			// check whether we just reduced the new leaf!
			if(color != f_max) {
				l->f_index = color;
				f_palette[color] = l;
			}
#else
			color = f_index;
			f_index++;
			l->f_index = color;
			// this needs to be done at a later time
			//f_palette[color] = l;
#endif
			return l;
		}
		if(l->f_type != 'B') {	// this is a terminal leave alright
			// if same group, we simple add the components & increase the counter
			if(l->f_group == leave.f_group || l->f_locked) {
				while(l->f_type == '>') {
					// this is a link, follow it
					l = l->f_extra;
				}
				l->f_count++;
				l->f_red   += leave.f_red;
				l->f_green += leave.f_green;
				l->f_blue  += leave.f_blue;
				l->f_alpha += leave.f_alpha;
				return l;
			}
			// ha! we need to create a new branch with these two entries
			p = AllocBranch();
			p->f_level = static_cast<unsigned char>(idx / 4);
			b->f_leaves[idx] = reinterpret_cast<moColorLeaf *>(p);
			p->f_parent = b;
			p->f_next = f_level[(unsigned char) p->f_level];
			if(p->f_next != 0) {
				p->f_next->f_previous = p;
			}
			f_level[(unsigned char) p->f_level] = p;
			idx = (l->f_group >> (level - 4)) & 15;
			p->f_leaves[idx] = l;
			b = p;
		}
		else {
			b = reinterpret_cast<moColorBranch *>(l);
		}
	} while(level > 0);
	/* this place should never be reached but if all the levels have been exhausted */
	throw moError(MO_ERROR_INTERNAL, "level reached zero before color was inserted in color tree (index = %ld, group = 0x%08X)", f_index, leave.f_group);
	/*NOTREACHED*/
}



unsigned char *moImage::Paletize8(unsigned long& count, moRGBA *& palette) const
{
	const moRGBA	*s;
	unsigned long	sz, idx;
	unsigned char	*d;
	moColorLeaf	leave, **l, **array, *p;
	moColorInfo	info;

// valid input?
	if(IsEmpty() || count < 16 || count > 256) {
		return 0;
	}

	array = new moColorLeaf *[f_size];
	info.f_palette = new moColorLeaf *[count];

	info.f_max = count;
	l = array;
	s = Data();
	sz = f_size;
	do {
		leave.Set(*s);
		*l = info.Insert(leave);
		s++;
		l++;
		sz--;
	} while(sz > 0);
printf("Insertions done! (allocated %ld bytes, %f kb for %ld colors)\n", info.f_allocated, (double) info.f_allocated / 1024.0, info.f_index);

	palette = new moRGBA[info.f_index];
	idx = info.f_index;
	while(idx > 0) {
		idx--;
		palette[idx] = info.f_palette[idx]->Get();
	}
printf("Palette done!\n");

	l = array;
	d = reinterpret_cast<unsigned char *>(l);
	sz = f_size;
	do {
		p = l[0];
		while(p->f_type == '>') {
			// get the link destination instead
			p = p->f_extra;
		}
		*d = static_cast<unsigned char>(p->f_index);
		d++;
		l++;
		sz--;
	} while(sz > 0);

	return reinterpret_cast<unsigned char *>(array);
}



unsigned short *moImage::Paletize16(unsigned long& count) const
{
	const moRGBA		*s;
	moArrayBase		*array;
	unsigned long		cnt, sz;
	unsigned short		*indexes, *d;
	moCountColor		count_color;
	moArray::position_t	p;

	if(IsEmpty() || count < 2 || count > 65536) {
		return 0;
	}

	indexes = new unsigned short[f_size];

	array = Histogram();
	cnt = array->Count();
	if(cnt <= count) {
		// there isn't more colors than what the user
		// asked for; so don't do any further quantization
		count = cnt;
		s = Data();
		sz = f_size;
		d = indexes;
		do {
			count_color.color = *s;
			p = array->Find(&count_color);
			// here p should always be defined in [0..count)
			if(p >= cnt) {
				// problem!
				throw moBug(MO_ERROR_INTERNAL, "pointer too large");
			}
			*d = static_cast<unsigned char>(p);
			d++;
			s++;
			sz--;
		} while(sz > 0);
		return indexes;
	}

	return indexes;
}





/************************************************************ DOC:

NAME

	moP2 - fast power of two computing

SYNOPSIS

	unsigned long moP2(unsigned long v);

PARAMETERS

	v - value to transform in a power of two

DESCRIPTION

	This function returns the closest logarithm of base 2 of the
	parameter value. The result is garantee to be large or equal
	to v up to 0x80000000.

RETURNED VALUE

	the power of 2 larger or equal to the input value if available

*/

unsigned long moP2(unsigned long v)
{
	if(v <= 0x00000001) return 0x00000001;
	if(v <= 0x00000002) return 0x00000002;
	if(v <= 0x00000004) return 0x00000004;
	if(v <= 0x00000008) return 0x00000008;
	if(v <= 0x00000010) return 0x00000010;
	if(v <= 0x00000020) return 0x00000020;
	if(v <= 0x00000040) return 0x00000040;
	if(v <= 0x00000080) return 0x00000080;
	if(v <= 0x00000100) return 0x00000100;
	if(v <= 0x00000200) return 0x00000200;
	if(v <= 0x00000400) return 0x00000400;
	if(v <= 0x00000800) return 0x00000800;
	if(v <= 0x00001000) return 0x00001000;
	if(v <= 0x00002000) return 0x00002000;
	if(v <= 0x00004000) return 0x00004000;
	if(v <= 0x00008000) return 0x00008000;
	if(v <= 0x00010000) return 0x00010000;
	if(v <= 0x00020000) return 0x00020000;
	if(v <= 0x00040000) return 0x00040000;
	if(v <= 0x00080000) return 0x00080000;
	if(v <= 0x00100000) return 0x00100000;
	if(v <= 0x00200000) return 0x00200000;
	if(v <= 0x00400000) return 0x00400000;
	if(v <= 0x00800000) return 0x00800000;
	if(v <= 0x01000000) return 0x01000000;
	if(v <= 0x02000000) return 0x02000000;
	if(v <= 0x04000000) return 0x04000000;
	if(v <= 0x08000000) return 0x08000000;
	if(v <= 0x10000000) return 0x10000000;
	if(v <= 0x20000000) return 0x20000000;
	if(v <= 0x40000000) return 0x40000000;
	if(v <= 0x80000000) return 0x80000000;

	return static_cast<unsigned long>(-1);
}

};			// namespace molib;

// vim: ts=8

