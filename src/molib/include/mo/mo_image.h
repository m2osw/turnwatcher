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



#ifndef MO_IMAGE_H
#define	MO_IMAGE_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_FILE_H
#include	"mo_file.h"
#endif
#ifndef MO_VARIABLE_H
#include	"mo_variable.h"
#endif
#ifndef MO_ARRAY_H
#include	"mo_array.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moRGBA;
class MO_DLL_EXPORT moRGBA16;
class MO_DLL_EXPORT moRGBAFloat;


class MO_DLL_EXPORT moRGB
{
public:
	unsigned char		red;
	unsigned char		green;
	unsigned char		blue;

				moRGB(void);
				moRGB(unsigned char r, unsigned char g, unsigned char b);
				moRGB(const moRGB& color);
				moRGB(const moRGBA& color);
				moRGB(const moRGBA16& color16);
				moRGB(const moRGBAFloat& color_float);
	void			Set(unsigned char r, unsigned char g, unsigned char b);
	unsigned char		Clamp(float value);
	moRGB&			operator = (const moRGB& color);
	moRGB&			operator = (const moRGBA& color);
	moRGB&			operator = (const moRGBA16& color16);
	moRGB&			operator = (const moRGBAFloat& color_float);
	bool			operator == (const moRGB& color) const;
	bool			operator == (const moRGBA& color) const;
	bool			operator == (const moRGBA16& color16) const;
	bool			operator == (const moRGBAFloat& color_float) const;
	bool			operator != (const moRGB& color) const;
	bool			operator != (const moRGBA& color) const;
	bool			operator != (const moRGBA16& color16) const;
	bool			operator != (const moRGBAFloat& color_float) const;
	bool			IsTransparent(void) const;
	bool			IsSolid(void) const;
	unsigned char		Grey(void) const;
	void			ApplyAlpha(void);
};



class MO_DLL_EXPORT moRGBA
{
public:
	unsigned char		red;
	unsigned char		green;
	unsigned char		blue;
	unsigned char		alpha;

				moRGBA(void);
				moRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				moRGBA(const moRGB& color);
				moRGBA(const moRGBA& color);
				moRGBA(const moRGBA16& color16);
				moRGBA(const moRGBAFloat& color_float);
	void			Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	unsigned char		Clamp(float value);
	moRGBA&			operator = (const moRGB& color);
	moRGBA&			operator = (const moRGBA& color);
	moRGBA&			operator = (const moRGBA16& color16);
	moRGBA&			operator = (const moRGBAFloat& color_float);
	bool			operator == (const moRGB& color) const;
	bool			operator == (const moRGBA& color) const;
	bool			operator == (const moRGBA16& color16) const;
	bool			operator == (const moRGBAFloat& color_float) const;
	bool			operator != (const moRGB& color) const;
	bool			operator != (const moRGBA& color) const;
	bool			operator != (const moRGBA16& color16) const;
	bool			operator != (const moRGBAFloat& color_float) const;
	bool			IsTransparent(void) const;
	bool			IsSolid(void) const;
	unsigned char		Grey(void) const;
	void			ApplyAlpha(void);
};



class MO_DLL_EXPORT moRGBA16
{
public:
	unsigned short		red;
	unsigned short		green;
	unsigned short		blue;
	unsigned short		alpha;

				moRGBA16(void);
				moRGBA16(unsigned short r, unsigned short g, unsigned short b, unsigned short a);
				moRGBA16(const moRGBA& color);
				moRGBA16(const moRGBA16& color16);
				moRGBA16(const moRGBAFloat& color_float);
	void			Set(unsigned short r, unsigned short g, unsigned short b, unsigned short a);
	unsigned short		Clamp(float value);
	moRGBA16&		operator = (const moRGBA& color);
	moRGBA16&		operator = (const moRGBA16& color16);
	moRGBA16&		operator = (const moRGBAFloat& color_float);
	bool			operator == (const moRGBA& color) const;
	bool			operator == (const moRGBA16& color16) const;
	bool			operator == (const moRGBAFloat& color_float) const;
	bool			operator != (const moRGBA& color) const;
	bool			operator != (const moRGBA16& color16) const;
	bool			operator != (const moRGBAFloat& color_float) const;
	bool			IsTransparent(void) const;
	bool			IsSolid(void) const;
	unsigned short		Grey(void) const;
	void			ApplyAlpha(void);
};



class MO_DLL_EXPORT moRGBAFloat
{
public:
	float			red;
	float			green;
	float			blue;
	float			alpha;

				moRGBAFloat(void);
				moRGBAFloat(float r, float g, float b, float a);
				moRGBAFloat(const moRGBA& color);
				moRGBAFloat(const moRGBA16& color16);
				moRGBAFloat(const moRGBAFloat& color_float);
	void			Set(float r, float g, float b, float a);
	float			Clamp(float value);
	moRGBAFloat&		operator = (const moRGBA& color);
	moRGBAFloat&		operator = (const moRGBA16& color16);
	moRGBAFloat&		operator = (const moRGBAFloat& color_float);
	bool			operator == (const moRGBA& color) const;
	bool			operator == (const moRGBA16& color16) const;
	bool			operator == (const moRGBAFloat& color_float) const;
	bool			operator != (const moRGBA& color) const;
	bool			operator != (const moRGBA16& color16) const;
	bool			operator != (const moRGBAFloat& color_float) const;
	bool			IsTransparent(void) const;
	bool			IsSolid(void) const;
	float			Grey(void) const;
	void			ApplyAlpha(void);
};




inline moRGB::moRGB(void)
{
}

inline moRGB::moRGB(unsigned char r, unsigned char g, unsigned char b)
{
	red =   r;
	green = g;
	blue =  b;
}
inline moRGB::moRGB(const moRGB& color)
{
	*this = color;
}
inline moRGB::moRGB(const moRGBA& color)
{
	red =   color.red;
	green = color.green;
	blue =  color.blue;
}
inline moRGB::moRGB(const moRGBA16& color16)
{
	red =   color16.red   >> 8;
	green = color16.green >> 8;
	blue =  color16.blue  >> 8;
}
inline moRGB::moRGB(const moRGBAFloat& color_float)
{
	red =   Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue =  Clamp(color_float.blue);
}
inline void moRGB::Set(unsigned char r, unsigned char g, unsigned char b)
{
	red =   r;
	green = g;
	blue =  b;
}
inline unsigned char moRGB::Clamp(float value)
{
	if(value <= 0.0f) {
		return 0;
	}
	if(value >= 1.0f) {
		return 255;
	}
	return (unsigned char) rint(value * 255.0f);
}
inline moRGB& moRGB::operator = (const moRGB& color)
{
	// In most cases people are reasonable and they don't
	// try to copy to this so we skip the test because in
	// many cases we will use this function to copy an
	// entire image in another and thus we are saving
	// a zillion useless tests
	//if(this != &color) {
		red   = color.red;
		green = color.green;
		blue  = color.blue;
	//}
	return *this;
}
inline moRGB& moRGB::operator = (const moRGBA& color)
{
	// In most cases people are reasonable and they don't
	// try to copy to this so we skip the test because in
	// many cases we will use this function to copy an
	// entire image in another and thus we are saving
	// a zillion useless tests
	//if(this != &color) {
		red   = color.red;
		green = color.green;
		blue  = color.blue;
	//}
	return *this;
}
inline moRGB& moRGB::operator = (const moRGBA16& color16)
{
	red =   color16.red   >> 8;
	green = color16.green >> 8;
	blue =  color16.blue  >> 8;
	return *this;
}
inline moRGB& moRGB::operator = (const moRGBAFloat& color_float)
{
	red =   Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue =  Clamp(color_float.blue);
	return *this;
}
inline bool moRGB::operator == (const moRGB& color) const
{
	return	   red   == color.red
		&& green == color.green
		&& blue  == color.blue;
}
inline bool moRGB::operator == (const moRGBA& color) const
{
	return	   red   == color.red
		&& green == color.green
		&& blue  == color.blue;
}
inline bool moRGB::operator == (const moRGBA16& color16) const
{
	return	   red   * 256 == color16.red
		&& green * 256 == color16.green
		&& blue  * 256 == color16.blue;
}
inline bool moRGB::operator == (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 255.0f == color_float.red
		&& (float) green / 255.0f == color_float.green
		&& (float) blue  / 255.0f == color_float.blue;
}
inline bool moRGB::operator != (const moRGB& color) const
{
	return	   red   != color.red
		|| green != color.green
		|| blue  != color.blue;
}
inline bool moRGB::operator != (const moRGBA& color) const
{
	return	   red   != color.red
		|| green != color.green
		|| blue  != color.blue;
}
inline bool moRGB::operator != (const moRGBA16& color16) const
{
	return	   red   * 256 != color16.red
		|| green * 256 != color16.green
		|| blue  * 256 != color16.blue;
}
inline bool moRGB::operator != (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 255.0f != color_float.red
		|| (float) green / 255.0f != color_float.green
		|| (float) blue  / 255.0f != color_float.blue;
}
inline bool moRGB::IsTransparent(void) const
{
	return false;
}
inline bool moRGB::IsSolid(void) const
{
	return true;
}
inline unsigned char moRGB::Grey(void) const
{
	return (6969 * red + 23434 * green + 2365 * blue) / 32768;
}
inline void moRGB::ApplyAlpha(void)
{
}




inline moRGBA::moRGBA(void)
{
}

inline moRGBA::moRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline moRGBA::moRGBA(const moRGB& color)
{
	red =   color.red;
	green = color.green;
	blue =  color.blue;
	alpha = 255;
}
inline moRGBA::moRGBA(const moRGBA& color)
{
	*this = color;
}
inline moRGBA::moRGBA(const moRGBA16& color16)
{
	red =   color16.red   >> 8;
	green = color16.green >> 8;
	blue =  color16.blue  >> 8;
	alpha = color16.alpha >> 8;
}
inline moRGBA::moRGBA(const moRGBAFloat& color_float)
{
	red =   Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue =  Clamp(color_float.blue);
	alpha = Clamp(color_float.alpha);
}
inline void moRGBA::Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline unsigned char moRGBA::Clamp(float value)
{
	if(value <= 0.0f) {
		return 0;
	}
	if(value >= 1.0f) {
		return 255;
	}
	return (unsigned char) rint(value * 255.0f);
}
inline moRGBA& moRGBA::operator = (const moRGB& color)
{
	red   = color.red;
	green = color.green;
	blue  = color.blue;
	alpha = 255;
	return *this;
}
inline moRGBA& moRGBA::operator = (const moRGBA& color)
{
	// In most cases people are reasonable and they don't
	// try to copy to this so we skip the test because in
	// many cases we will use this function to copy an
	// entire image in another and thus we are saving
	// a zillion useless tests
	//if(this != &color) {
		red   = color.red;
		green = color.green;
		blue  = color.blue;
		alpha = color.alpha;
	//}
	return *this;
}
inline moRGBA& moRGBA::operator = (const moRGBA16& color16)
{
	red =   color16.red   >> 8;
	green = color16.green >> 8;
	blue =  color16.blue  >> 8;
	alpha = color16.alpha >> 8;
	return *this;
}
inline moRGBA& moRGBA::operator = (const moRGBAFloat& color_float)
{
	red =   Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue =  Clamp(color_float.blue);
	alpha = Clamp(color_float.alpha);
	return *this;
}
inline bool moRGBA::operator == (const moRGB& color) const
{
	return	   red   == color.red
		&& green == color.green
		&& blue  == color.blue
		&& alpha == 255;
}
inline bool moRGBA::operator == (const moRGBA& color) const
{
	return	   red   == color.red
		&& green == color.green
		&& blue  == color.blue
		&& alpha == color.alpha;
}
inline bool moRGBA::operator == (const moRGBA16& color16) const
{
	return	   red   * 256 == color16.red
		&& green * 256 == color16.green
		&& blue  * 256 == color16.blue
		&& alpha * 256 == color16.alpha;
}
inline bool moRGBA::operator == (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 255.0f == color_float.red
		&& (float) green / 255.0f == color_float.green
		&& (float) blue  / 255.0f == color_float.blue
		&& (float) alpha / 255.0f == color_float.alpha;
}
inline bool moRGBA::operator != (const moRGB& color) const
{
	return	   red   != color.red
		|| green != color.green
		|| blue  != color.blue
		|| alpha != 255;
}
inline bool moRGBA::operator != (const moRGBA& color) const
{
	return	   red   != color.red
		|| green != color.green
		|| blue  != color.blue
		|| alpha != color.alpha;
}
inline bool moRGBA::operator != (const moRGBA16& color16) const
{
	return	   red   * 256 != color16.red
		|| green * 256 != color16.green
		|| blue  * 256 != color16.blue
		|| alpha * 256 != color16.alpha;
}
inline bool moRGBA::operator != (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 255.0f != color_float.red
		|| (float) green / 255.0f != color_float.green
		|| (float) blue  / 255.0f != color_float.blue
		|| (float) alpha / 255.0f != color_float.alpha;
}
inline bool moRGBA::IsTransparent(void) const
{
	return alpha == 0;
}
inline bool moRGBA::IsSolid(void) const
{
	return alpha == 255;
}
inline unsigned char moRGBA::Grey(void) const
{
	return (6969 * red + 23434 * green + 2365 * blue) / 32768;
}
inline void moRGBA::ApplyAlpha(void)
{
	if(alpha == 0) {
		red = green = blue = 0;
	}
	else if(alpha != 255) {
		red   = red   * alpha / 255;
		green = green * alpha / 255;
		blue  = blue  * alpha / 255;
	}
}




inline moRGBA16::moRGBA16(void)
{
}
inline moRGBA16::moRGBA16(unsigned short r, unsigned short g, unsigned short b, unsigned short a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline moRGBA16::moRGBA16(const moRGBA& color)
{
	red =   color.red   << 8;
	green = color.green << 8;
	blue =  color.blue  << 8;
	alpha = color.alpha << 8;
}
inline moRGBA16::moRGBA16(const moRGBA16& color16)
{
	*this = color16;
}
inline moRGBA16::moRGBA16(const moRGBAFloat& color_float)
{
	red =   Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue =  Clamp(color_float.blue);
	alpha = Clamp(color_float.alpha);
}
inline void moRGBA16::Set(unsigned short r, unsigned short g, unsigned short b, unsigned short a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline unsigned short moRGBA16::Clamp(float value)
{
	if(value <= 0.0f) {
		return 0;
	}
	if(value >= 1.0f) {
		return 65535;
	}
	return (unsigned short) rint(value * 65535.0f);
}
inline moRGBA16& moRGBA16::operator = (const moRGBA& color)
{
	red =   color.red   << 8;
	green = color.green << 8;
	blue =  color.blue  << 8;
	alpha = color.alpha << 8;
	return *this;
}
inline moRGBA16& moRGBA16::operator = (const moRGBA16& color16)
{
	if(this != &color16) {
		red   = color16.red;
		green = color16.green;
		blue  = color16.blue;
		alpha = color16.alpha;
	}
	return *this;
}
inline moRGBA16& moRGBA16::operator = (const moRGBAFloat& color_float)
{
	red   = Clamp(color_float.red);
	green = Clamp(color_float.green);
	blue  = Clamp(color_float.blue);
	alpha = Clamp(color_float.alpha);
	return *this;
}
inline bool moRGBA16::operator == (const moRGBA& color) const
{
	return	   red   == color.red   * 256
		&& green == color.green * 256
		&& blue  == color.blue  * 256
		&& alpha == color.alpha * 256;
}
inline bool moRGBA16::operator == (const moRGBA16& color16) const
{
	return	   red   == color16.red
		&& green == color16.green
		&& blue  == color16.blue
		&& alpha == color16.alpha;
}
inline bool moRGBA16::operator == (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 65535.0f == color_float.red
		&& (float) green / 65535.0f == color_float.green
		&& (float) blue  / 65535.0f == color_float.blue
		&& (float) alpha / 65535.0f == color_float.alpha;
}
inline bool moRGBA16::operator != (const moRGBA& color) const
{
	return	   red   != color.red   * 256
		|| green != color.green * 256
		|| blue  != color.blue  * 256
		|| alpha != color.alpha * 256;
}
inline bool moRGBA16::operator != (const moRGBA16& color16) const
{
	return	   red   != color16.red
		|| green != color16.green
		|| blue  != color16.blue
		|| alpha != color16.alpha;
}
inline bool moRGBA16::operator != (const moRGBAFloat& color_float) const
{
	return	   (float) red   / 65535.0f != color_float.red
		|| (float) green / 65535.0f != color_float.green
		|| (float) blue  / 65535.0f != color_float.blue
		|| (float) alpha / 65535.0f != color_float.alpha;
}
inline bool moRGBA16::IsTransparent(void) const
{
	return alpha == 0;
}
inline bool moRGBA16::IsSolid(void) const
{
	return alpha == 65535;
}
inline unsigned short moRGBA16::Grey(void) const
{
	return static_cast<unsigned short>(((unsigned long) 6969 * (unsigned long) red
		+ (unsigned long) 23434 * (unsigned long) green
			+ (unsigned long) 2365 * (unsigned long) blue) / (unsigned long) 32768);
}
inline void moRGBA16::ApplyAlpha(void)
{
	if(alpha == 0) {
		red = green = blue = 0;
	}
	else if(alpha != 65535) {
		red   = static_cast<unsigned short>((unsigned long) red   * (unsigned long) alpha / 65535);
		green = static_cast<unsigned short>((unsigned long) green * (unsigned long) alpha / 65535);
		blue  = static_cast<unsigned short>((unsigned long) blue  * (unsigned long) alpha / 65535);
	}
}




inline moRGBAFloat::moRGBAFloat(void)
{
}
inline moRGBAFloat::moRGBAFloat(float r, float g, float b, float a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline moRGBAFloat::moRGBAFloat(const moRGBA& color)
{
	red =   (float) color.red   / 255.0f;
	green = (float) color.green / 255.0f;
	blue =  (float) color.blue  / 255.0f;
	alpha = (float) color.alpha / 255.0f;
}
inline moRGBAFloat::moRGBAFloat(const moRGBA16& color16)
{
	red =   (float) color16.red   / 65535.0f;
	green = (float) color16.green / 65535.0f;
	blue =  (float) color16.blue  / 65535.0f;
	alpha = (float) color16.alpha / 65535.0f;
}
inline moRGBAFloat::moRGBAFloat(const moRGBAFloat& color_float)
{
	*this = color_float;
}
inline void moRGBAFloat::Set(float r, float g, float b, float a)
{
	red =   r;
	green = g;
	blue =  b;
	alpha = a;
}
inline float moRGBAFloat::Clamp(float value)
{
	if(value <= 0.0f) {
		return 0;
	}
	if(value >= 1.0f) {
		return 1.0f;
	}
	return value;
}
inline moRGBAFloat& moRGBAFloat::operator = (const moRGBA& color)
{
	red =   (float) color.red   / 255.0f;
	green = (float) color.green / 255.0f;
	blue =  (float) color.blue  / 255.0f;
	alpha = (float) color.alpha / 255.0f;
	return *this;
}
inline moRGBAFloat& moRGBAFloat::operator = (const moRGBA16& color16)
{
	red   = (float) color16.red   / 65535.0f;
	green = (float) color16.green / 65535.0f;
	blue  = (float) color16.blue  / 65535.0f;
	alpha = (float) color16.alpha / 65535.0f;
	return *this;
}
inline moRGBAFloat& moRGBAFloat::operator = (const moRGBAFloat& color_float)
{
	if(this != &color_float) {
		red   = color_float.red;
		green = color_float.green;
		blue  = color_float.blue;
		alpha = color_float.alpha;
	}
	return *this;
}
inline bool moRGBAFloat::operator == (const moRGBA& color) const
{
	return	   red   == (float) color.red   / 255.0f
		&& green == (float) color.green / 255.0f
		&& blue  == (float) color.blue  / 255.0f
		&& alpha == (float) color.alpha / 255.0f;
}
inline bool moRGBAFloat::operator == (const moRGBA16& color16) const
{
	return	   red   == (float) color16.red   / 65535.0f
		&& green == (float) color16.green / 65535.0f
		&& blue  == (float) color16.blue  / 65535.0f
		&& alpha == (float) color16.alpha / 65535.0f;
}
inline bool moRGBAFloat::operator == (const moRGBAFloat& color_float) const
{
	return	   red   == color_float.red
		&& green == color_float.green
		&& blue  == color_float.blue
		&& alpha == color_float.alpha;
}
inline bool moRGBAFloat::operator != (const moRGBA& color) const
{
	return	   red   != (float) color.red   / 255.0f
		|| green != (float) color.green / 255.0f
		|| blue  != (float) color.blue  / 255.0f
		|| alpha != (float) color.alpha / 255.0f;
}
inline bool moRGBAFloat::operator != (const moRGBA16& color16) const
{
	return	   red   != (float) color16.red   / 65535.0f
		|| green != (float) color16.green / 65535.0f
		|| blue  != (float) color16.blue  / 65535.0f
		|| alpha != (float) color16.alpha / 65535.0f;
}
inline bool moRGBAFloat::operator != (const moRGBAFloat& color_float) const
{
	return	   red   != color_float.red
		|| green != color_float.green
		|| blue  != color_float.blue
		|| alpha != color_float.alpha;
}
inline bool moRGBAFloat::IsTransparent(void) const
{
	return alpha <= 0.0f;
}
inline bool moRGBAFloat::IsSolid(void) const
{
	return alpha >= 1.0f;
}
inline float moRGBAFloat::Grey(void) const
{
	return 0.212677f * red + 0.715148f * green + 0.072174f * blue;
}
inline void moRGBAFloat::ApplyAlpha(void)
{
	if(alpha == 0.0f) {
		red = green = blue = 0.0f;
	}
	else if(alpha != 1.0f) {
		red   = red   * alpha;
		green = green * alpha;
		blue  = blue  * alpha;
	}
}






struct MO_DLL_EXPORT moCountColor {
	unsigned long	count;
	moRGBA		color;
};

struct MO_DLL_EXPORT moCountColor16 {
	unsigned long	count;
	moRGBA16	color;
};



struct MO_DLL_EXPORT moColorGroup {
	unsigned long	f_group;		// group number
	unsigned long	f_count;		// number of items in this group
	moCountColor	f_color_count;		// the count + color of this entry

	void		Set(const moCountColor& color_count);
};



struct MO_DLL_EXPORT moColorLeaf {
	char			f_type;			// 'L' or '>' (leave or link)
	char			f_locked;		// if 1, can't create sub-branches
	unsigned long		f_group;		// the exact value of this specific leave
	unsigned long		f_index;		// this color index in the resulting palette
	unsigned long		f_count;		// when type == '>' a pointer to another leave
	unsigned long		f_red;
	unsigned long		f_green;
	unsigned long		f_blue;
	unsigned long		f_alpha;
	struct moColorLeaf *	f_extra;

	void		Set(const moRGBA& color);
	moRGBA		Get(void) const;
};



struct MO_DLL_EXPORT moColorBranch {
	char			f_type;		// 'B' (branch)
	unsigned char		f_level;	// this branch level
	struct moColorBranch *	f_parent;	// if 0 that's the root node
	struct moColorBranch *	f_next;		// next at the same level
	struct moColorBranch *	f_previous;	// previous branch at the same level
	moColorLeaf *		f_leaves[16];	// link to one level below (a leaf or a branch)

	void		Init(void);
};



struct MO_DLL_EXPORT moColorBuffer {
	struct moColorBuffer *	f_next;
};


struct MO_DLL_EXPORT moColorInfo {
	moColorBranch *	f_level[8];	// first branch of each level (going from 7 to 0)
	moColorBranch	f_root;		// the root node
	moColorBuffer *	f_branches;	// a list of branches
	moColorBranch *	f_next_branch;	// the next free branch
	moColorBuffer *	f_leaves;	// a list of leaves
	moColorLeaf *	f_next_leaf;	// the next free leaf
	moColorLeaf **	f_palette;	// this is actually an array of moRGBA
	unsigned long	f_index;	// the current index
	unsigned long	f_max;		// maximum number of colors (if index > max then reduce)
	unsigned long	f_allocated;	// total size of memory allocted

			moColorInfo(void);
			~moColorInfo();
	moColorBranch *	AllocBranch(void);
	void		FreeBranch(moColorBranch *branch);
	moColorLeaf *	AllocLeaf(void);
	moColorLeaf *	Insert(const moColorLeaf& leave);
	moColorLeaf *	Simplify(moColorBranch *branch);
	unsigned long	Reduce(void);
};





class MO_DLL_EXPORT moImageBase : public moBase
{
public:
	enum alpha_test_t {
		ALPHA_TEST_COMBINAISON = 0,
		ALPHA_TEST_SOLID,
		ALPHA_TEST_TRANSPARENT,
		ALPHA_TEST_EMPTY	// empty images
	};

	static const unsigned long	MAX_WIDTH  = 16384;
	static const unsigned long	MAX_HEIGHT = 16384;
	static const unsigned long	RESIZE_MODE_PICTURE = 0x00000000;
	static const unsigned long	RESIZE_MODE_LINES = 0x00000001;

				moImageBase(void);
	virtual			~moImageBase();

// The SetSize() is used to create a RAM image
	void			SetSize(unsigned long width, unsigned long height, bool safe = false);

// The SetData() will be used to create a Hardware image (i.e. a bitmap
// saved directly in video RAM) -- this is not currently supported
	//void			SetData(unsigned long width, unsigned long height, unsigned long bpr, void *data);

	bool			IsEmpty(void) const;
	bool			IsTransparent(void) const;
	bool			IsSolid(void) const;
	bool			IsCombinaison(void) const;
	virtual alpha_test_t	AlphaTest(void) const = 0;
	virtual void		ApplyAlpha(void) = 0;
	unsigned long		Width(void) const;
	unsigned long		WidthP2(void);
	unsigned long		Height(void) const;
	unsigned long		HeightP2(void);
	unsigned long		Depth(void) const;
	unsigned long		BytesPerRow(void) const;
	unsigned long		Size(void) const;
	void			Resize(unsigned long new_width, unsigned long new_height, unsigned long mode);
	const moVariableList&	Parameters(void) const;
	moVariableList&		Parameters(void);
	int			LastErrno(int new_errno = -1) const;

// If you want a pointer to the image data, you probably want to use
// the specific Data() of the type of image you are using...
	unsigned char *		RawData(void);
	const unsigned char *	RawData(void) const;

protected:
	virtual void		NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height) = 0;

	moVariableList		f_parameters;	// a list of parameters about this image
	unsigned long		f_width;
	unsigned long		f_width_p2;	// f_width at the next power of 2 (0 - still undefined)
	unsigned long		f_height;
	unsigned long		f_height_p2;	// f_height at the next power of 2 (0 - still undefined)
	unsigned long		f_depth;	// the depth of a pixel
	unsigned long		f_size;		// width * height
	unsigned long		f_max;		// size of the f_data buffer
	mutable int		f_errno;	// last error
	unsigned char *		f_data;		// the image data

private:
	void			Init(void);
};


class MO_DLL_EXPORT moImage16;
class MO_DLL_EXPORT moImageFloat;

class MO_DLL_EXPORT moImage : public moImageBase
{
public:
				moImage(void);
				moImage(const moImage& im);
				moImage(const moImage16& im);
				moImage(const moImageFloat& im);
				moImage(unsigned long width, unsigned long height);
				moImage(unsigned long width, unsigned long height, const moRGBA& clear);
	virtual			~moImage();

	void			Clear(const moRGBA& clear, long x, long y, unsigned long width, unsigned long height);
	const moRGBA *		Data(void) const;
	moRGBA *		Data(void);
	virtual alpha_test_t	AlphaTest(void) const;
	virtual void		ApplyAlpha(void);
	moArrayBase *		Histogram(void) const;
	unsigned char *		Paletize8(unsigned long& count, moRGBA *& palette) const;
	unsigned short *	Paletize16(unsigned long& count) const;
	void			OpenGLTexCompatible(void);	// the height becomes a power of two

	moImage&		operator = (const moImage& im);
	moImage&		operator = (const moImage16& im);
	moImage&		operator = (const moImageFloat& im);

// the following are pretty generic functions
	//static void		Copy(moRGBA *destination, unsigned long dw, unsigned long dh, const moRGBA *source, unsigned long sw, unsigned long sh, long x, long y, long sx, long sy, unsigned long width, unsigned long height);

private:
	virtual void		NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height);
};

typedef moSmartPtr<moImage>	moImageSPtr;





class MO_DLL_EXPORT moImage16 : public moImageBase
{
public:
				moImage16(void);
				moImage16(const moImage& im);
				moImage16(const moImage16& im);
				moImage16(const moImageFloat& im);
				moImage16(unsigned long width, unsigned long height);
				moImage16(unsigned long width, unsigned long height, const moRGBA16& clear);
	virtual			~moImage16();

	void			Clear(const moRGBA16& clear, long x, long y, unsigned long width, unsigned long height);
	const moRGBA16 *	Data(void) const;
	moRGBA16 *		Data(void);
	virtual alpha_test_t	AlphaTest(void) const;
	virtual void		ApplyAlpha(void);
	moArrayBase *		Histogram(void) const;
	void			OpenGLTexCompatible(void);	// the height becomes a power of two

	moImage16&		operator = (const moImage& im);
	moImage16&		operator = (const moImage16& im);
	moImage16&		operator = (const moImageFloat& im);

// the following are pretty generic functions
	//static void		Copy(moRGBA16 *destination, unsigned long dw, unsigned long dh, const moRGBA16 *source, unsigned long sw, unsigned long sh, long x, long y, long sx, long sy, unsigned long width, unsigned long height);

private:
	virtual void		NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height);
};

typedef moSmartPtr<moImage16>	moImage16SPtr;







class MO_DLL_EXPORT moImageFloat : public moImageBase
{
public:
				moImageFloat(void);
				moImageFloat(const moImage& im);
				moImageFloat(const moImage16& im);
				moImageFloat(const moImageFloat& im);
				moImageFloat(unsigned long width, unsigned long height);
				moImageFloat(unsigned long width, unsigned long height, const moRGBAFloat& clear);
	virtual			~moImageFloat();

	void			Clear(const moRGBAFloat& clear, long x, long y, unsigned long width, unsigned long height);
	const moRGBAFloat *	Data(void) const;
	moRGBAFloat *		Data(void);
	virtual alpha_test_t	AlphaTest(void) const;
	virtual void		ApplyAlpha(void);
	moArrayBase *		Histogram(void) const;
	void			OpenGLTexCompatible(void);	// the height becomes a power of two

	moImageFloat&		operator = (const moImage& im);
	moImageFloat&		operator = (const moImage16& im);
	moImageFloat&		operator = (const moImageFloat& im);

// the following are pretty generic functions
	//static void		Copy(moRGBAFloat *destination, unsigned long dw, unsigned long dh, const moRGBAFloat *source, unsigned long sw, unsigned long sh, long x, long y, long sx, long sy, unsigned long width, unsigned long height);

private:
	virtual void		NewSizeCopy(unsigned char *data, unsigned long width, unsigned long height);
};

typedef moSmartPtr<moImageFloat>	moImageFloatSPtr;




template<class D, class S>
MO_DLL_EXPORT_FUNC void CopyImageData(D *destination, unsigned long dw, unsigned long dh, const S *source, unsigned long sw, unsigned long sh, long x, long y, long sx, long sy, unsigned long width, unsigned long height)
{
	long		w;

	if(x < 0) {
		sx -= x;
		width += x;
		x = 0;
	}
	if(y < 0) {
		sy -= y;
		height += y;
		y = 0;
	}
	if(sx < 0) {
		x -= sx;
		width += sx;
		sx = 0;
	}
	if(sy < 0) {
		y -= sy;
		height += sy;
		sy = 0;
	}

	// coordinates outside the image or sizes of 0 or less?
	if(source == 0 || destination == 0
	|| static_cast<unsigned long>(x)  >= dw || static_cast<unsigned long>(y)  >= dh
	|| static_cast<unsigned long>(sx) >= sw || static_cast<unsigned long>(sy) >= sh
	|| static_cast<long>(width) <= 0 || static_cast<long>(height) <= 0) {
		return;
	}

	if(x + width > dw) {
		width = dw - x;
	}
	if(y + height > dh) {
		height = dh - y;
	}

	if(sx + width > sw) {
		width = sw - sx;
	}
	if(sy + height > sh) {
		height = sh - sy;
	}

	// TODO: by specializing a few of these templates we could
	//	 have faster versions using MMX and SSE
	destination += y * dw;
	source += sy * sw;
	dw -= width;
	sw -= width;
	while(height > 0) {
		height--;
		w = width;
		while(w > 0) {
			w--;
			*destination = *source;
			destination++;
			source++;
		}
		destination += dw;
		source      += sw;
	}
}



template<class D>
MO_DLL_EXPORT_FUNC void VerticalFlipImageData(D *data, unsigned long width, unsigned long height)
{
	unsigned long	w;
	D		c, *s1, *s2;

	// coordinates outside the image or sizes of 0 or less?
	if(data == 0 || static_cast<long>(width) <= 0 || static_cast<long>(height) <= 0) {
		return;
	}

	s1 = data;
	s2 = data + width * height;
	height /= 2;
	while(height > 0) {
		--height;
		// swap one line
		w = width;
		s2 -= width;
		while(w > 0) {
			--w;
			// swap two pixels
			c = *s1;
			*s1 = *s2;
			*s2 = c;
			++s1;
			++s2;
		}
		s2 -= width;
	}
}










template<class D, class C>
MO_DLL_EXPORT_FUNC void ClearImage(D *d, unsigned long dw, unsigned long dh, const C& clear, long x, long y, unsigned long width, unsigned long height)
{
	long		w;

	if(x < 0) {
		width += x;
		x = 0;
	}
	if(y < 0) {
		height += y;
		y = 0;
	}

	// coordinates outside the image or sizes of 0 or less?
	if(static_cast<unsigned long>(x) >= dw || static_cast<unsigned long>(y) >= dh
	|| static_cast<long>(width) <= 0 || static_cast<long>(height) <= 0) {
		return;
	}

	if(x + width > dw) {
		width = dw - x;
	}
	if(y + height > dh) {
		height = dh - y;
	}

	// TODO: create an assembly version of the following using MMX
	//	 (need to specialized a few templates for that)
	D color(clear);
	d += y * dw;
	while(height > 0) {
		height--;
		w = width;
		while(w > 0) {
			w--;
			*d = color;
			d++;
		}
		d += dw - width;
	}
}










class MO_DLL_EXPORT moImageFile : public moBase
{
public:
	enum mo_image_file_depth_t {
		// determine the DEPTH parameter using the DefineDepth() function
		MO_IMAGE_FILE_DEPTH_UNKNOWN = 0,
		MO_IMAGE_FILE_DEPTH_BLACKNWHITE,
		MO_IMAGE_FILE_DEPTH_PALETTED,
		MO_IMAGE_FILE_DEPTH_GREYSCALE,
		MO_IMAGE_FILE_DEPTH_RED,
		MO_IMAGE_FILE_DEPTH_GREEN,
		MO_IMAGE_FILE_DEPTH_BLUE,
		MO_IMAGE_FILE_DEPTH_ALPHA,
		MO_IMAGE_FILE_DEPTH_GREYA,
		MO_IMAGE_FILE_DEPTH_RGB555,
		MO_IMAGE_FILE_DEPTH_RGB5551,
		MO_IMAGE_FILE_DEPTH_RGB,
		MO_IMAGE_FILE_DEPTH_RGBA,
		MO_IMAGE_FILE_DEPTH_RGBX,
		MO_IMAGE_FILE_DEPTH_max
	};

	enum mo_image_file_compression_t {
		// determine the COMPRESSION parameter using the DefineCompression() function
		MO_IMAGE_FILE_COMPRESSION_UNKNOWN = 0,
		MO_IMAGE_FILE_COMPRESSION_NONE,
		MO_IMAGE_FILE_COMPRESSION_BEST,
		MO_IMAGE_FILE_COMPRESSION_TEST,
		MO_IMAGE_FILE_COMPRESSION_RLE,
		MO_IMAGE_FILE_COMPRESSION_max
	};



				moImageFile(void);
	virtual			~moImageFile();

	virtual const char *	MimeType(void) const = 0;
	virtual long		MatchPriority(void) const = 0;
	virtual long		MatchType(const void *buffer, unsigned long size) const = 0;
	virtual bool		Load(moIStream& file, moImage& im) = 0;
	virtual bool		Save(moOStream& file, const moImage& im) = 0;
	virtual bool		Load(moIStream& file, moImage16& im);
	virtual bool		Save(moOStream& file, const moImage16& im);
	virtual bool		Load(moIStream& file, moImageFloat& im);
	virtual bool		Save(moOStream& file, const moImageFloat& im);

	compare_t		ComparePriority(const moBase& object) const;

	static mo_image_file_depth_t DefineDepth(const moImage& im, bool test_solid = false);
	static mo_image_file_compression_t DefineCompression(const moImage& im);

private:
	virtual compare_t	Compare(const moBase& object) const;
};

typedef moSmartDynamicPtr<moImageFile>			moImageFileSPtr;
typedef moTmplList<moImageFile, moSortedList>		moSortedListOfImageFile;
typedef moTmplList<moImageFile, moSortedListUnique>	moSortedListUniqueOfImageFile;
typedef moSmartDynamicPtr<moSortedListOfImageFile>	moSortedListOfImageFileSPtr;
typedef moSmartDynamicPtr<moSortedListUniqueOfImageFile> moSortedListUniqueOfImageFileSPtr;



class MO_DLL_EXPORT moImageFileFactory;
typedef moSmartPtr<moImageFileFactory>	moImageFileFactorySPtr;

class MO_DLL_EXPORT moImageFileFactory : public moBase
{
public:
	class MO_DLL_EXPORT moImageFileRegister
	{
	public:
						moImageFileRegister(void)
						{
							f_next = 0;
							moImageFileFactory::GetFactory()->Register(*this);
						}
		virtual				~moImageFileRegister()
						{
						}
		void				SetNext(moImageFileRegister *next)
						{
							f_next = next;
						}
		moImageFileRegister *		GetNext(void) const
						{
							return const_cast<moImageFileRegister *>(f_next);
						}
		virtual moImageFileSPtr		Create(void) const = 0;
	private:
		moImageFileRegister *		f_next;
	};

	void				Register(moImageFileRegister& format);
	moSortedListUniqueOfImageFileSPtr GetMIMEList(void) const;
	moSortedListOfImageFileSPtr	GetMatchList(void) const;

	static moImageFileFactorySPtr	GetFactory(void);
	static void			Done(void);

	virtual const char *		moGetClassName(void) const;

private:
					moImageFileFactory(void);
	virtual				~moImageFileFactory();

	void				InitLists(void) const;

	moImageFileRegister *		f_registers;
	mutable moSortedListUniqueOfImageFileSPtr f_mime_list;
	mutable moSortedListOfImageFileSPtr f_match_list;
	mutable moMutex			f_mutex;

	static moImageFileFactory *	g_factory;
	static bool			g_done;
};


typedef	moImageFile *	(*mo_imagefile_func_t)(void);

#if 0
// this was cool stuff, but it doesn't work well with .so files...
#define	MAKE_IMAGEFILE_PUBLIC(name)					\
	extern "C" {							\
		moImageFile *NewImageFile_##name(void)			\
		{ return new moImageFile_##name; }			\
		const mo_imagefile_func_t __imagefile_tab_##name	\
			__attribute__((section("_mo_imagefile_tab")))	\
				= NewImageFile_##name;			\
	}
#endif

#define	MAKE_IMAGEFILE_PUBLIC(name)					\
	class MO_DLL_EXPORT moImageFileRegister_##name : public moImageFileFactory::moImageFileRegister {	\
		virtual moImageFileSPtr Create(void) const {		\
			return new moImageFile_##name;			\
		}							\
	};								\
	static moImageFileRegister_##name mo_imagefile_register_##name;

// Add the following constructor to the previous class definition to get
// the list of registered image file formats.
//public: moImageFileRegister_##name() { fprintf(stderr, "registered " #name "\n"); }


MO_DLL_EXPORT_FUNC extern	const unsigned char	mo5BitsTo8Bits[32];
MO_DLL_EXPORT_FUNC extern	unsigned long		moP2(unsigned long v);
MO_DLL_EXPORT_FUNC extern	bool			moLoadImage(moIStream& file, moImage& im);
MO_DLL_EXPORT_FUNC extern	bool			moLoadImage(moIStream& file, moImage16& im);
MO_DLL_EXPORT_FUNC extern	bool			moLoadImage(moIStream& file, moImageFloat& im);
MO_DLL_EXPORT_FUNC extern	bool			moSaveImage(moOStream& file, const moImage& im, const char *mime_type = 0);
MO_DLL_EXPORT_FUNC extern	bool			moSaveImage(moOStream& file, const moImage16& im, const char *mime_type = 0);
MO_DLL_EXPORT_FUNC extern	bool			moSaveImage(moOStream& file, const moImageFloat& im, const char *mime_type = 0);
MO_DLL_EXPORT_FUNC extern	moImageFileSPtr		moFindImageFile(const char *mime_type);
MO_DLL_EXPORT_FUNC extern	moImageFileSPtr		moFindImageFile(const void *buffer, unsigned long size);






/************************************************************ DOC:

NAME

	moValidImageSize - return true if the sizes are supported

SYNOPSIS

	inline bool moValidImageSize(long width, long height);

DESCRIPTION

	This function compares the width and height of an image
	(usually to be loaded in memory) and returns true if
	they are accepted.

	Note that an empty image is valid, however sizes of
	zero (0) are not accepted as being valid.

RETURN VALUE

	true if both dimensions are in the range 1 to maximum internal
	size

SEE ALSO

	moLoadImage, moSaveImage

*/
inline bool moValidImageSize(long width, long height)
{
	// note: an image in 64 bits per pixel of 32767 x 32767 won't fit in 4Go...
	// also an image of 32 bits per pixel will, it's unlikely you will be
	// able to allocate such an amount of memory
	// the current limit enables images of up to 2Go in memory in 64 bits
	// which is already very unlikely going to work
	return width > 0 && height > 0
		&& width <= static_cast<long>(moImageBase::MAX_WIDTH)
		&& height <= static_cast<long>(moImageBase::MAX_HEIGHT);
}



/************************************************************ DOC:

CLASS

	moImageBase

NAME

	LastErrno - return the last error which occured

SYNOPSIS

	inline int LastErrno(int new_errno = -1) const;

PARAMETERS

	new_errno - the new error number or -1 to leave unchanged

DESCRIPTION

	When a function applied to an image fails, it generates
	an error which is saved within the image structure.

	By default the function only returns the last error.
	It is also possible to modify the error number by
	setting an error number other than -1.

	Also when loading or saving an image file an error can
	occur.  This error will be saved in the image structure
	since there is a place holder for this.

	Most function won't reset the last error value. It
	is a good idea to reset the error before to call a
	function to know whether that function failed by
	checking the error number. This can be done in
	this way:

		(void) LastErrno(0);

RETURN VALUE

	the last error (one of the MO_ERROR_... error)

SEE ALSO

	moLoadImage, moSaveImage

*/
inline int moImageBase::LastErrno(int new_errno) const
{
	int	r;

	r = f_errno;
	if(new_errno != -1) {
		f_errno = new_errno;
	}

	return r;
}



/************************************************************ DOC:

CLASS

	moImageBase

NAME

	RawData -- return a direct pointer to the image data

SYNOPSIS

	unsigned char *RawData(void);
	const unsigned char *RawData(void) const;

DESCRIPTION

	These functions return a direct pointer to the data of
	the image. It is suggested that you use Data() instead
	whenever possible.

RETURN VALUE

	a pointer to a buffer of unsigned data

SEE ALSO

	Data

*/
inline unsigned char *moImageBase::RawData(void)
{
	return f_data;
}

inline const unsigned char *moImageBase::RawData(void) const
{
	return f_data;
}




/************************************************************ DOC:

CLASS

	moImageBase
	moImage
	moImage16
	moImageFloat

NAME

	IsEmpty - check whether this image is empty
	Width - return the width of the image
	Height - return the height of the image
	WidthP2 - return the next width which is a power of 2
	HeightP2 - return the next height which is a power of 2
	Size - return the size of the image data (i.e. width x height)
	Depth - return the size in bytes of one pixel

SYNOPSIS

	bool IsEmpty(void) const;
	unsigned long Width(void) const;
	unsigned long Height(void) const;
	unsigned long WidthP2(void);
	unsigned long HeightP2(void);
	unsigned long Size(void) const;
	unsigned long Depth(void) const;

DESCRIPTION

	The IsEmpty() function returns true if the image is empty
	(has sizes of zero and/or no data).

	The other functions return the size of the image. The Width()
	and Height() functions return the exact sizes. The WidthP2()
	and HeightP2() functions return the next power of two of the
	Width() and Height() functions.

	The Size() function returns the size in pixels of the data.
	It will be the same whatever the format. It is usually used
	to know how many pixels to copy in a function or another.

	The Depth() function returns the size in bytes of a pixel.
	This is defined in each specific format and it returns
	sizeof(<pixel>) -- i.e. sizeof(moRGBA) for moImage.

WARNINGS

	The Depth() function will NOT work in the constructor of the
	moImageBase class (or, rather, the depth will still be zero
	at that time.)

RETURN VALUE

	return a size of the image or whether it is empty

SEE ALSO

	Constructors, Crop

*/
inline bool moImageBase::IsEmpty(void) const
{
	return f_width == 0 || f_height == 0;
}


inline unsigned long moImageBase::Width(void) const
{
	return f_width;
}


inline unsigned long moImageBase::Height(void) const
{
	return f_height;
}


inline unsigned long moImageBase::WidthP2(void)
{
	if(f_width_p2 == 0) {
		return f_width_p2 = moP2(f_width);
	}

	return f_width_p2;
}


inline unsigned long moImageBase::HeightP2(void)
{
	if(f_height_p2 == 0) {
		return f_height_p2 = moP2(f_height);
	}

	return f_height_p2;
}


inline unsigned long moImageBase::Size(void) const
{
	return f_size;
}


inline unsigned long moImageBase::Depth(void) const
{
	return f_depth;
}





/************************************************************ DOC:

CLASS

	moImageBase

NAME

	Parameters - return a pointer on the parameters

SYNOPSIS

	const moVariableList& Parameters(void) const;
	moVariableList& Parameters(void);

DESCRIPTION

	These functions retrieve a reference on the list of
	parameters available in the given image.

	Some parameters will usually be set by the Load()
	funtion and used by the Save() function in order
	to avoid losing the information available in
	image files. It can also be used to insert comments
	in the different file formats which support it.

RETURN VALUE

	return a reference to the parameters of the image

SEE ALSO

	Constructors, Load, Save

*/
inline const moVariableList& moImageBase::Parameters(void) const
{
	return f_parameters;
}


inline moVariableList& moImageBase::Parameters(void)
{
	return f_parameters;
}




/************************************************************ DOC:

CLASS

	moImage
	moImage16

NAME

	Data - return a pointer to the data of the image

SYNOPSIS

	const moRGBA *Data(void) const;
	moRGBA *Data(void);
	const moRGBA16 *Data(void) const;
	moRGBA16 *Data(void);
	const moRGBAFloat *Data(void) const;
	moRGBAFloat *Data(void);

DESCRIPTION

	These functions will be used to directly access the data of
	an image. The moImage function uses the moRGBA type,
	the moImage16 uses the moRGBA16 type and the moImageFloat
	uses the moRGBAFloat type.

	The moRGBA16 is defined by the corresponding structure.
	The endian of the data depends of the endian of your
	computer. Thus, shorts can directly be used as shorts.

	Similarly, the moRGBAFloat type is defined by the
	corresponding structure and the endian of your computer
	is used to save these floats.

RETURN VALUE

	return a pointer to the bottom-right of the image which
	is the start of the buffer (the images are bottom-top).

	If the image is empty, this function will return a
	null pointer.

SEE ALSO

	SetSize

*/
inline const moRGBA *moImage::Data(void) const
{
	return reinterpret_cast<const moRGBA *>(f_data);
}


inline moRGBA *moImage::Data(void)
{
	return reinterpret_cast<moRGBA *>(f_data);
}


inline const moRGBA16 *moImage16::Data(void) const
{
	return reinterpret_cast<const moRGBA16 *>(f_data);
}


inline moRGBA16 *moImage16::Data(void)
{
	return reinterpret_cast<moRGBA16 *>(f_data);
}


inline const moRGBAFloat *moImageFloat::Data(void) const
{
	return reinterpret_cast<const moRGBAFloat *>(f_data);
}


inline moRGBAFloat *moImageFloat::Data(void)
{
	return reinterpret_cast<moRGBAFloat *>(f_data);
}







};			// namespace molib;

// vim: ts=8 sw=8
#endif	// #ifndef MO_IMAGE_H

