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

	moImageFile_BMP

NAME

	Load - load an image from a file
	Save - save an image to a file

SYNOPSIS

	virtual bool Load(moIStream& file, moImage& im) = 0;
	virtual bool Save(moOStream& file, const moImage& im) = 0;

PARAMETERS

	file - a file where the image is read or written
	im - the image loaded or to be saved

DESCRIPTION

	This documentation is supplemental to the main moImageFile
	documentation of the same functions. This is used to give
	a list of all the parameters understood by this handler.


	Load()

	[RELAX]		if this parameter exists, it makes this
			loader accept erroneous files and return
			a valid (but most probably incomplet)
			image

	"COMPRESSION"	"RLE", "NONE"

	"DEPTH"		"PALETTED", "GREYSCALE", "RGB555", "RGB5551", "RGB", "RGBA"

	"COMMENT"	<string>
			(arbitrary user comment)


	Save()

	"COMMENT"	<string>
			(arbitrary user comment)

	"COMPRESSION"	"RLE"
			(other values are taken as NONE)

	"DEPTH"		"GREY", "RGB555", "RGB", "RGBA"
			(other values are taken as RGB if the
			image is solid, RGBA otherwise)

	"INTERLACE"	1, 2, 4 or 8
			(the number of lines to skip)

SEE ALSO

	MatchType, MatchPosition, MimeType

*/



#if 0

/*
http://entropymine.com/jason/bmpsuite/


typedef struct tagBITMAPCOREHEADER {
     DWORD   bcSize; 
     WORD    bcWidth; 
     WORD    bcHeight; 
     WORD    bcPlanes; 
     WORD    bcBitCount; 
} BITMAPCOREHEADER, *PBITMAPCOREHEADER; 

typedef struct _BITMAPCOREINFO { 
     BITMAPCOREHEADER  bmciHeader; 
     RGBTRIPLE         bmciColors[1]; 
} BITMAPCOREINFO, *PBITMAPCOREINFO; 

typedef struct tagBITMAPFILEHEADER { 
     WORD    bfType; 
     DWORD   bfSize; 
     WORD    bfReserved1; 
     WORD    bfReserved2; 
     DWORD   bfOffBits; 
} BITMAPFILEHEADER, *PBITMAPFILEHEADER; 

typedef struct tagBITMAPINFO { 
     BITMAPINFOHEADER bmiHeader; 
     RGBQUAD          bmiColors[1]; 
} BITMAPINFO, *PBITMAPINFO; 

typedef struct tagBITMAPINFOHEADER{
     DWORD  biSize; 
     LONG   biWidth; 
     LONG   biHeight; 
     WORD   biPlanes; 
     WORD   biBitCount; 
     DWORD  biCompression; 
     DWORD  biSizeImage; 
     LONG   biXPelsPerMeter; 
     LONG   biYPelsPerMeter; 
     DWORD  biClrUsed; 
     DWORD  biClrImportant; 
} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 





   BITMAPINFOHEADER

   The BITMAPINFOHEADER structure contains information about the dimensions and color format of a DIB. 

   Windows 95, Windows NT 4.0: Applications can use the BITMAPV4HEADER structure for added functionality. 

   Windows 98/Me, Windows 2000/XP: Applications can use the BITMAPV5HEADER structure for added functionality. However, these are used only in
   the CreateDIBitmap function.

   typedef struct tagBITMAPINFOHEADER{
     DWORD  biSize; 
     LONG   biWidth; 
     LONG   biHeight; 
     WORD   biPlanes; 
     WORD   biBitCount; 
     DWORD  biCompression; 
     DWORD  biSizeImage; 
     LONG   biXPelsPerMeter; 
     LONG   biYPelsPerMeter; 
     DWORD  biClrUsed; 
     DWORD  biClrImportant; 
   } BITMAPINFOHEADER, *PBITMAPINFOHEADER; 

   Members

   biSize 
        Specifies the number of bytes required by the structure. 
   biWidth 
        Specifies the width of the bitmap, in pixels. 

        Windows 98/Me, Windows 2000/XP: If biCompression is BI_JPEG or BI_PNG, the biWidth member specifies the width of the decompressed
        JPEG or PNG image file, respectively. 
   biHeight 
        Specifies the height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the
        lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. 

        If biHeight is negative, indicating a top-down DIB, biCompression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be
        compressed. 

        Windows 98/Me, Windows 2000/XP: If biCompression is BI_JPEG or BI_PNG, the biHeight member specifies the height of the decompressed
        JPEG or PNG image file, respectively. 
   biPlanes 
        Specifies the number of planes for the target device. This value must be set to 1. 
   biBitCount 
        Specifies the number of bits-per-pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that
        define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values. 
         Value
                         Meaning
         0
                         Windows 98/Me, Windows 2000/XP: The number of bits-per-pixel is specified or is implied by the JPEG or PNG
                         format. 
         1
                         The bitmap is monochrome, and the bmiColors member of BITMAPINFO contains two entries. Each bit in the bitmap
                         array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the
                         bmiColors table; if the bit is set, the pixel has the color of the second entry in the table.
         4
                         The bitmap has a maximum of 16 colors, and the bmiColors member of BITMAPINFO contains up to 16 entries. Each
                         pixel in the bitmap is represented by a 4-bit index into the color table. For example, if the first byte in the
                         bitmap is 0x1F, the byte represents two pixels. The first pixel contains the color in the second table entry, and
                         the second pixel contains the color in the sixteenth table entry.
         8
                         The bitmap has a maximum of 256 colors, and the bmiColors member of BITMAPINFO contains up to 256 entries. In
                         this case, each byte in the array represents a single pixel.
         16
                         The bitmap has a maximum of 2^16 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the
                         bmiColors member of BITMAPINFO is NULL. Each WORD in the bitmap array represents a single pixel. The relative
                         intensities of red, green, and blue are represented with five bits for each color component. The value for blue
                         is in the least significant five bits, followed by five bits each for green and red. The most significant bit is
                         not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain
                         the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER. 

                         If the biCompression member of the BITMAPINFOHEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components, respectively, of each pixel. Each WORD in the
                         bitmap array represents a single pixel.

                         Windows NT/Windows 2000/XP: When the biCompression member is BI_BITFIELDS, bits set in each DWORD mask must be
                         contiguous and should not overlap the bits of another mask. All the bits in the pixel do not have to be used. 

                         Windows 95/98/Me: When the biCompression member is BI_BITFIELDS, the system supports only the following 16bpp
                         color masks: A 5-5-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x03E0, and the red mask is
                         0x7C00; and a 5-6-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x07E0, and the red mask is
                         0xF800.
         24
                         The bitmap has a maximum of 2^24 colors, and the bmiColors member of BITMAPINFO is NULL. Each 3-byte triplet in
                         the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The
                         bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of
                         entries specified by the biClrUsed member of the BITMAPINFOHEADER. 
         32
                         The bitmap has a maximum of 2^32 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the
                         bmiColors member of BITMAPINFO is NULL. Each DWORD in the bitmap array represents the relative intensities of
                         blue, green, and red, respectively, for a pixel. The high byte in each DWORD is not used. The bmiColors color
                         table is used for optimizing colors used on palette-based devices, and must contain the number of entries
                         specified by the biClrUsed member of the BITMAPINFOHEADER. 

                         If the biCompression member of the BITMAPINFOHEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components, respectively, of each pixel. Each DWORD in the
                         bitmap array represents a single pixel.

                         Windows NT/ 2000: When the biCompression member is BI_BITFIELDS, bits set in each DWORD mask must be contiguous
                         and should not overlap the bits of another mask. All the bits in the pixel do not need to be used.

                         Windows 95/98/Me: When the biCompression member is BI_BITFIELDS, the system supports only the following 32-bpp
                         color mask: The blue mask is 0x000000FF, the green mask is 0x0000FF00, and the red mask is 0x00FF0000. 


   biCompression 
        Specifies the type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). This member can be one of
        the following values. 
         Value
                                            Description
         BI_RGB
                                            An uncompressed format.
         BI_RLE8
                                            A run-length encoded (RLE) format for bitmaps with 8 bpp. The compression format is a
                                            2-byte format consisting of a count byte followed by a byte containing a color index. For
                                            more information, see Bitmap Compression. 
         BI_RLE4
                                            An RLE format for bitmaps with 4 bpp. The compression format is a 2-byte format consisting
                                            of a count byte followed by two word-length color indexes. For more information, see
                                            Bitmap Compression.
         BI_BITFIELDS
                                            Specifies that the bitmap is not compressed and that the color table consists of three
                                            DWORD color masks that specify the red, green, and blue components, respectively, of each
                                            pixel. This is valid when used with 16- and 32-bpp bitmaps.
         BI_JPEG
                                            Windows 98/Me, Windows 2000/XP: Indicates that the image is a JPEG image.
         BI_PNG
                                            Windows 98/Me, Windows 2000/XP: Indicates that the image is a PNG image.


   biSizeImage 
        Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. 

        Windows 98/Me, Windows 2000/XP: If biCompression is BI_JPEG or BI_PNG, biSizeImage indicates the size of the JPEG or PNG image
        buffer, respectively. 
   biXPelsPerMeter 
        Specifies the horizontal resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to
        select a bitmap from a resource group that best matches the characteristics of the current device. 
   biYPelsPerMeter 
        Specifies the vertical resolution, in pixels-per-meter, of the target device for the bitmap. 
   biClrUsed 
        Specifies the number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses
        the maximum number of colors corresponding to the value of the biBitCount member for the compression mode specified by biCompression.

        If biClrUsed is nonzero and the biBitCount member is less than 16, the biClrUsed member specifies the actual number of colors the
        graphics engine or device driver accesses. If biBitCount is 16 or greater, the biClrUsed member specifies the size of the color table
        used to optimize performance of the system color palettes. If biBitCount equals 16 or 32, the optimal color palette starts
        immediately following the three DWORD masks. 

        When the bitmap array immediately follows the BITMAPINFO structure, it is a packed bitmap. Packed bitmaps are referenced by a single
        pointer. Packed bitmaps require that the biClrUsed member must be either zero or the actual size of the color table. 
   biClrImportant 
        Specifies the number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required. 

   Remarks

   The BITMAPINFO structure combines the BITMAPINFOHEADER structure and a color table to provide a complete definition of the dimensions and
   colors of a DIB. For more information about DIBs, see Device-Independent Bitmaps and BITMAPINFO. 

   An application should use the information stored in the biSize member to locate the color table in a BITMAPINFO structure, as follows: 

   pColor = ((LPSTR)pBitmapInfo + (WORD)(pBitmapInfo->bmiHeader.biSize));

   Windows 98/Me, Windows 2000/XP: The BITMAPINFOHEADER structure is extended to allow a JPEG or PNG image to be passed as the source image to
   StretchDIBits. 

   Requirements 

     Windows NT/2000/XP: Included in Windows NT 3.1 and later.
     Windows 95/98/Me: Included in Windows 95 and later.
     Header: Declared in Wingdi.h; include Windows.h.









   BITMAPV4HEADER

   The BITMAPV4HEADER structure is the bitmap information header file. It is an extended version of the BITMAPINFOHEADER structure. 

   Windows NT 3.51 and earlier: Applications should continue to use BITMAPINFOHEADER. 

   Windows 98/Me, Windows 2000/XP: Applications can use the BITMAPV5HEADER structure for added functionality. 

   typedef struct {
     DWORD        bV4Size;
     LONG         bV4Width;
     LONG         bV4Height;
     WORD         bV4Planes;
     WORD         bV4BitCount;
     DWORD        bV4V4Compression;
     DWORD        bV4SizeImage;
     LONG         bV4XPelsPerMeter;
     LONG         bV4YPelsPerMeter;
     DWORD        bV4ClrUsed;
     DWORD        bV4ClrImportant;
     DWORD        bV4RedMask;
     DWORD        bV4GreenMask;
     DWORD        bV4BlueMask;
     DWORD        bV4AlphaMask;
     DWORD        bV4CSType;
     CIEXYZTRIPLE bV4Endpoints;
     DWORD        bV4GammaRed;
     DWORD        bV4GammaGreen;
     DWORD        bV4GammaBlue;
   } BITMAPV4HEADER, *PBITMAPV4HEADER; 

   Members

   bV4Size 
        Specifies the number of bytes required by the structure. Applications should use this member to determine which bitmap information
        header structure is being used. 
   bV4Width 
        Specifies the width of the bitmap, in pixels. 

        Windows 98/Me, Windows 2000/XP: If bV4Compression is BI_JPEG or BI_PNG, bV4Width specifies the width of the JPEG or PNG image in
        pixels. 
   bV4Height 
        Specifies the height of the bitmap, in pixels. If bV4Height is positive, the bitmap is a bottom-up DIB and its origin is the
        lower-left corner. If bV4Height is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. 

        If bV4Height is negative, indicating a top-down DIB, bV4Compression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be
        compressed. 

        Windows 98/Me, Windows 2000/XP: If bV4Compression is BI_JPEG or BI_PNG, bV4Height specifies the height of the JPEG or PNG image in
        pixels. 
   bV4Planes 
        Specifies the number of planes for the target device. This value must be set to 1. 
   bV4BitCount 
        Specifies the number of bits-per-pixel. The bV4BitCount member of the BITMAPV4HEADER structure determines the number of bits that
        define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values. 
         Value
                         Meaning
         0
                         Windows 98/Me, Windows 2000/XP: The number of bits-per-pixel is specified or is implied by the JPEG or PNG file
                         format. 
         1
                         The bitmap is monochrome, and the bmiColors member of BITMAPINFO contains two entries. Each bit in the bitmap
                         array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the
                         bmiColors table; if the bit is set, the pixel has the color of the second entry in the table.
         4
                         The bitmap has a maximum of 16 colors, and the bmiColors member of BITMAPINFO contains up to 16 entries. Each
                         pixel in the bitmap is represented by a 4-bit index into the color table. For example, if the first byte in the
                         bitmap is 0x1F, the byte represents two pixels. The first pixel contains the color in the second table entry, and
                         the second pixel contains the color in the sixteenth table entry.
         8
                         The bitmap has a maximum of 256 colors, and the bmiColors member of BITMAPINFO contains up to 256 entries. In
                         this case, each byte in the array represents a single pixel.
         16
                         The bitmap has a maximum of 2^16 colors. If the bV4Compression member of the BITMAPV4HEADER structure is BI_RGB,
                         the bmiColors member of BITMAPINFO is NULL. Each WORD in the bitmap array represents a single pixel. The relative
                         intensities of red, green, and blue are represented with five bits for each color component. The value for blue
                         is in the least significant five bits, followed by five bits each for green and red, respectively. The most
                         significant bit is not used. The bmiColors color table is used for optimizing colors used on palette-based
                         devices, and must contain the number of entries specified by the bV4ClrUsed member of the BITMAPV4HEADER. 

                         If the bV4Compression member of the BITMAPV4HEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components of each pixel. Each WORD in the bitmap array
                         represents a single pixel.
         24
                         The bitmap has a maximum of 2^24 colors, and the bmiColors member of BITMAPINFO is NULL. Each 3-byte triplet in
                         the bitmap array represents the relative intensities of blue, green, and red for a pixel. The bmiColors color
                         table is used for optimizing colors used on palette-based devices, and must contain the number of entries
                         specified by the bV4ClrUsed member of the BITMAPV4HEADER. 
         32
                         The bitmap has a maximum of 2^32 colors. If the bV4Compression member of the BITMAPV4HEADER is BI_RGB, the
                         bmiColors member of BITMAPINFO is NULL. Each DWORD in the bitmap array represents the relative intensities of
                         blue, green, and red for a pixel. The high byte in each DWORD is not used. The bmiColors color table is used for
                         optimizing colors used on palette-based devices, and must contain the number of entries specified by the
                         biClrUsed member of the BITMAPV4HEADER. 

                         If the bV4Compression member of the BITMAPV4HEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components of each pixel. Each DWORD in the bitmap array
                         represents a single pixel.


   bV4V4Compression 
        Specifies the type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). This member can be one of
        the following values. 
         Value
                                     Description
         BI_RGB
                                     An uncompressed format.
         BI_RLE8
                                     A run-length encoded (RLE) format for bitmaps with 8 bpp. The compression format is a 2-byte
                                     format consisting of a count byte followed by a byte containing a color index. For more
                                     information, see Bitmap Compression.
         BI_RLE4
                                     An RLE format for bitmaps with 4 bpp. The compression format is a 2-byte format consisting of a
                                     count byte followed by two word-length color indexes. For more information, see Bitmap
                                     Compression.
         BI_BITFIELDS
                                     Specifies that the bitmap is not compressed. The members bV4RedMask, bV4GreenMask, and bV4BlueMask
                                     specify the red, green, and blue components for each pixel. This is valid when used with 16- and
                                     32-bpp bitmaps.
         BI_JPEG
                                     Windows 98/Me, Windows 2000/XP: Specifies that the image is compressed using the JPEG file
                                     interchange format. JPEG compression trades off compression against loss; it can achieve a
                                     compression ratio of 20:1 with little noticeable loss. 
         BI_PNG
                                     Windows 98/Me, Windows 2000/XP: Specifies that the image is compressed using the PNG file
                                     interchange format. 


   bV4SizeImage 
        Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. 

        Windows 98/Me, Windows 2000/XP: If bV4Compression is BI_JPEG or BI_PNG, bV4SizeImage is the size of the JPEG or PNG image buffer. 
   bV4XPelsPerMeter 
        Specifies the horizontal resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to
        select a bitmap from a resource group that best matches the characteristics of the current device. 
   bV4YPelsPerMeter 
        Specifies the vertical resolution, in pixels-per-meter, of the target device for the bitmap. 
   bV4ClrUsed 
        Specifies the number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses
        the maximum number of colors corresponding to the value of the bV4BitCount member for the compression mode specified by
        bV4Compression. 

        If bV4ClrUsed is nonzero and the bV4BitCount member is less than 16, the bV4ClrUsed member specifies the actual number of colors the
        graphics engine or device driver accesses. If bV4BitCount is 16 or greater, the bV4ClrUsed member specifies the size of the color
        table used to optimize performance of the system color palettes. If bV4BitCount equals 16 or 32, the optimal color palette starts
        immediately following the BITMAPV4HEADER. 
   bV4ClrImportant 
        Specifies the number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are important. 
   bV4RedMask 
        Color mask that specifies the red component of each pixel, valid only if bV4Compression is set to BI_BITFIELDS. 
   bV4GreenMask 
        Color mask that specifies the green component of each pixel, valid only if bV4Compression is set to BI_BITFIELDS. 
   bV4BlueMask 
        Color mask that specifies the blue component of each pixel, valid only if bV4Compression is set to BI_BITFIELDS. 
   bV4AlphaMask 
        Color mask that specifies the alpha component of each pixel. 
   bV4CSType 
        Specifies the color space of the DIB. The following table lists the value for bV4CSType. 
         Value
                                                             Meaning
         LCS_CALIBRATED_RGB
                                                             This value indicates that endpoints and gamma values are given in the
                                                             appropriate fields. 


        See the LOGCOLORSPACE structure for information that defines a logical color space. 
   bV4Endpoints 
        A CIEXYZTRIPLE structure that specifies the x, y, and z coordinates of the three colors that correspond to the red, green, and blue
        endpoints for the logical color space associated with the bitmap. This member is ignored unless the bV4CSType member specifies
        LCS_CALIBRATED_RGB. 

        Note  A color space is a model for representing color numerically in terms of three or more coordinates. For example, the RGB color
        space represents colors in terms of the red, green, and blue coordinates. 
   bV4GammaRed 
        Toned response curve for red. This member is ignored unless color values are calibrated RGB values and bV4CSType is set to
        LCS_CALIBRATED_RGB. Specified in 16^16 format. 
   bV4GammaGreen 
        Toned response curve for green. Used if bV4CSType is set to LCS_CALIBRATED_RGB. Specified as 16^16 format. 
   bV4GammaBlue 
        Toned response curve for blue. Used if bV4CSType is set to LCS_CALIBRATED_RGB. Specified as 16^16 format. 

   Remarks

   Windows 98/Me, Windows 2000/XP: The BITMAPV4HEADER structure is extended to allow a JPEG or PNG image to be passed as the source image to
   StretchDIBits.





   BITMAPV5HEADER

   The BITMAPV5HEADER structure is the bitmap information header file. It is an extended version of the BITMAPINFOHEADER structure.

   Windows 95 and Windows NT 4.0: Applications can use the BITMAPV4HEADER structure. 

   Windows NT 3.51 and earlier: Applications should continue to use BITMAPINFOHEADER. 

   typedef struct { 
     DWORD        bV5Size; 
     LONG         bV5Width; 
     LONG         bV5Height; 
     WORD         bV5Planes; 
     WORD         bV5BitCount; 
     DWORD        bV5Compression; 
     DWORD        bV5SizeImage; 
     LONG         bV5XPelsPerMeter; 
     LONG         bV5YPelsPerMeter; 
     DWORD        bV5ClrUsed; 
     DWORD        bV5ClrImportant; 
     DWORD        bV5RedMask; 
     DWORD        bV5GreenMask; 
     DWORD        bV5BlueMask; 
     DWORD        bV5AlphaMask; 
     DWORD        bV5CSType; 
     CIEXYZTRIPLE bV5Endpoints; 
     DWORD        bV5GammaRed; 
     DWORD        bV5GammaGreen; 
     DWORD        bV5GammaBlue; 
     DWORD        bV5Intent; 
     DWORD        bV5ProfileData; 
     DWORD        bV5ProfileSize; 
     DWORD        bV5Reserved; 
   } BITMAPV5HEADER, *PBITMAPV5HEADER; 

   Members

   bV5Size 
        Specifies the number of bytes required by the structure. Applications should use this member to determine which bitmap information
        header structure is being used. 
   bV5Width 
        Specifies the width of the bitmap, in pixels. 

        If bV5Compression is BI_JPEG or BI_PNG, the bV5Width member specifies the width of the decompressed JPEG or PNG image in pixels. 
   bV5Height 
        Specifies the height of the bitmap, in pixels. If the value of bV5Height is positive, the bitmap is a bottom-up DIB and its origin is
        the lower-left corner. If bV5Height value is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. 

        If bV5Height is negative, indicating a top-down DIB, bV5Compression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be
        compressed. 

        If bV5Compression is BI_JPEG or BI_PNG, the bV5Height member specifies the height of the decompressed JPEG or PNG image in pixels. 
   bV5Planes 
        Specifies the number of planes for the target device. This value must be set to 1. 
   bV5BitCount 
        Specifies the number of bits that define each pixel and the maximum number of colors in the bitmap. 

        This member can be one of the following values. 
         Value
                         Meaning
         0
                         The number of bits per pixel is specified or is implied by the JPEG or PNG file format.
         1
                         The bitmap is monochrome, and the bmiColors member of BITMAPINFO contains two entries. Each bit in the bitmap
                         array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the
                         bmiColors color table. If the bit is set, the pixel has the color of the second entry in the table.
         4
                         The bitmap has a maximum of 16 colors, and the bmiColors member of BITMAPINFO contains up to 16 entries. Each
                         pixel in the bitmap is represented by a 4-bit index into the color table. For example, if the first byte in the
                         bitmap is 0x1F, the byte represents two pixels. The first pixel contains the color in the second table entry, and
                         the second pixel contains the color in the sixteenth table entry.
         8
                         The bitmap has a maximum of 256 colors, and the bmiColors member of BITMAPINFO contains up to 256 entries. In
                         this case, each byte in the array represents a single pixel.
         16
                         The bitmap has a maximum of 2^16 colors. If the bV5Compression member of the BITMAPV5HEADER structure is BI_RGB,
                         the bmiColors member of BITMAPINFO is NULL. Each WORD in the bitmap array represents a single pixel. The relative
                         intensities of red, green, and blue are represented with five bits for each color component. The value for blue
                         is in the least significant five bits, followed by five bits each for green and red. The most significant bit is
                         not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain
                         the number of entries specified by the bV5ClrUsed member of the BITMAPV5HEADER. 

                         If the bV5Compression member of the BITMAPV5HEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components, respectively, of each pixel. Each WORD in the
                         bitmap array represents a single pixel.

                         When the bV5iCompression member is BI_BITFIELDS, bits set in each DWORD mask must be contiguous and should not
                         overlap the bits of another mask. All the bits in the pixel do not need to be used. 
         24
                         The bitmap has a maximum of 2^24 colors, and the bmiColors member of BITMAPINFO is NULL. Each 3-byte triplet in
                         the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The
                         bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of
                         entries specified by the bV5ClrUsed member of the BITMAPV5HEADER structure. 
         32
                         The bitmap has a maximum of 2^32 colors. If the bV5Compression member of the BITMAPV5HEADER is BI_RGB, the
                         bmiColors member of BITMAPINFO is NULL. Each DWORD in the bitmap array represents the relative intensities of
                         blue, green, and red, respectively, for a pixel. The high byte in each DWORD is not used. The bmiColors color
                         table is used for optimizing colors used on palette-based devices, and must contain the number of entries
                         specified by the bV5ClrUsed member of the BITMAPV5HEADER. 

                         If the bV5Compression member of the BITMAPV5HEADER is BI_BITFIELDS, the bmiColors member contains three DWORD
                         color masks that specify the red, green, and blue components of each pixel. Each DWORD in the bitmap array
                         represents a single pixel.


   bV5Compression 
        Specifies that the bitmap is not compressed. The bV5RedMask, bV5GreenMask, and bV5BlueMask members specify the red, green, and blue
        components of each pixel. This is valid when used with 16- and 32-bpp bitmaps. This member can be one of the following values. 
         Value
                                   Meaning
         BI_RGB
                                   An uncompressed format.
         BI_RLE8
                                   A run-length encoded (RLE) format for bitmaps with 8 bpp. The compression format is a two-byte format
                                   consisting of a count byte followed by a byte containing a color index. If bV5Compression is BI_RGB
                                   and the bV5BitCount member is 16, 24, or 32, the bitmap array specifies the actual intensities of
                                   blue, green, and red rather than using color table indexes. For more information, see Bitmap
                                   Compression.
         BI_RLE4
                                   An RLE format for bitmaps with 4 bpp. The compression format is a two-byte format consisting of a
                                   count byte followed by two word-length color indexes. For more information, see Bitmap Compression.
         BI_BITFIELDS
                                   Specifies that the bitmap is not compressed and that the color table consists of three DWORD color
                                   masks that specify the red, green, and blue components of each pixel. Valid when used with 16- and
                                   32-bpp bitmaps.
         BI_JPEG
                                   Specifies that the image is compressed using the JPEG file Interchange Format. JPEG compression
                                   trades off compression against loss; it can achieve a compression ratio of 20:1 with little
                                   noticeable loss. 
         BI_PNG
                                   Specifies that the image is compressed using the PNG file Interchange Format.


   bV5SizeImage 
        Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. 

        If bV5Compression is BI_JPEG or BI_PNG, bV5SizeImage is the size of the JPEG or PNG image buffer. 
   bV5XPelsPerMeter 
        Specifies the horizontal resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to
        select a bitmap from a resource group that best matches the characteristics of the current device. 
   bV5YPelsPerMeter 
        Specifies the vertical resolution, in pixels-per-meter, of the target device for the bitmap. 
   bV5ClrUsed 
        Specifies the number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses
        the maximum number of colors corresponding to the value of the bV5BitCount member for the compression mode specified by
        bV5Compression. 

        If bV5ClrUsed is nonzero and bV5BitCount is less than 16, the bV5ClrUsed member specifies the actual number of colors the graphics
        engine or device driver accesses. If bV5BitCount is 16 or greater, the bV5ClrUsed member specifies the size of the color table used
        to optimize performance of the system color palettes. If bV5BitCount equals 16 or 32, the optimal color palette starts immediately
        following the BITMAPV5HEADER. If bV5ClrUsed is nonzero, the color table is used on palettized devices, and bV5ClrUsed specifies the
        number of entries. 
   bV5ClrImportant 
        Specifies the number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required. 
   bV5RedMask 
        Color mask that specifies the red component of each pixel, valid only if bV5Compression is set to BI_BITFIELDS. 
   bV5GreenMask 
        Color mask that specifies the green component of each pixel, valid only if bV5Compression is set to BI_BITFIELDS. 
   bV5BlueMask 
        Color mask that specifies the blue component of each pixel, valid only if bV5Compression is set to BI_BITFIELDS. 
   bV5AlphaMask 
        Color mask that specifies the alpha component of each pixel. 
   bV5CSType 
        Specifies the color space of the DIB. 

        The following table specifies the values for bV5CSType. 
         Value
                                                                      Meaning
         LCS_CALIBRATED_RGB
                                                                      This value implies that endpoints and gamma values are given
                                                                      in the appropriate fields.
         LCS_sRGB
                                                                      Specifies that the bitmap is in sRGB color space. 
         LCS_WINDOWS_COLOR_SPACE
                                                                      This value indicates that the bitmap is in the system
                                                                      default color space, sRGB. 
         PROFILE_LINKED
                                                                      This value indicates that bV5ProfileData points to the file
                                                                      name of the profile to use (gamma and endpoints values are
                                                                      ignored). 
         PROFILE_EMBEDDED
                                                                      This value indicates that bV5ProfileData points to a memory
                                                                      buffer that contains the profile to be used (gamma and
                                                                      endpoints values are ignored).


        See the LOGCOLORSPACE structure for information that defines a logical color space. 
   bV5Endpoints 
        A CIEXYZTRIPLE structure that specifies the x, y, and z coordinates of the three colors that correspond to the red, green, and blue
        endpoints for the logical color space associated with the bitmap. This member is ignored unless the bV5CSType member specifies
        LCS_CALIBRATED_RGB. 
   bV5GammaRed 
        Toned response curve for red. Used if bV5CSType is set to LCS_CALIBRATED_RGB. Specified in 16^16 format. 
   bV5GammaGreen 
        Toned response curve for green. Used if bV5CSType is set to LCS_CALIBRATED_RGB. Specified in 16^16 format. 
   bV5GammaBlue 
        Toned response curve for blue. Used if bV5CSType is set to LCS_CALIBRATED_RGB. Specified in 16^16 format. 
   bV5Intent 
        Rendering intent for bitmap. This can be one of the following values. 
         Value
                                               Intent
                                                                 ICC name
                                                                                          Meaning
         LCS_GM_ABS_COLORIMETRIC
                                               Match
                                                                 Absolute Colorimetric
                                                                                          Maintains the white point. Matches
                                                                                          the colors to their nearest color in
                                                                                          the destination gamut.
         LCS_GM_BUSINESS
                                               Graphic
                                                                 Saturation
                                                                                          Maintains saturation. Used for
                                                                                          business charts and other situations
                                                                                          in which undithered colors are
                                                                                          required.
         LCS_GM_GRAPHICS
                                               Proof
                                                                 Relative Colorimetric
                                                                                          Maintains colorimetric match. Used
                                                                                          for graphic designs and named colors.
         LCS_GM_IMAGES
                                               Picture
                                                                 Perceptual
                                                                                          Maintains contrast. Used for
                                                                                          photographs and natural images.


   bV5ProfileData 
        The offset, in bytes, from the beginning of the BITMAPV5HEADER structure to the start of the profile data. If the profile is
        embedded, profile data is the actual profile, and it is linked. (The profile data is the null-terminated file name of the profile.)
        This cannot be a Unicode string. It must be composed exclusively of characters from the Windows character set (code page 1252). These
        profile members are ignored unless the bV5CSType member specifies PROFILE_LINKED or PROFILE_EMBEDDED. 
   bV5ProfileSize 
        Size, in bytes, of embedded profile data. 
   bV5Reserved 
        This member has been reserved for future use. Its value should be set to zero. 

   Remarks

   If bV5Height is negative, indicating a top-down DIB, bV5Compression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be
   compressed.

   The Independent Color Management interface (ICM) 2.0 allows International Color Consortium (ICC) color profiles to be linked or embedded in
   DIBs (DIBs). See Using Structures in ICM 2.0 for more information. 

   When a DIB is loaded into memory, the profile data (if present) should follow the color table, and the bV5ProfileData should provide the
   offset of the profile data from the beginning of the BITMAPV5HEADER structure. The value stored in bV5ProfileDate will be different from the
   value returned by the sizeof operator given the BITMAPV5HEADER argument, because bV5ProfileData is the offset in bytes from the beginning of
   the BITMAPV5HEADER structure to the start of the profile data. (Bitmap bits do not follow the color table in memory). Applications should
   modify the bV5ProfileData member after loading the DIB into memory. 

   For packed DIBs, the profile data should follow the bitmap bits similar to the file format. The bV5ProfileData member should still give the
   offset of the profile data from the beginning of the BITMAPV5HEADER. 

   Applications should access the profile data only when bV5Size equals the size of the BITMAPV5HEADER and bV5CSType equals PROFILE_EMBEDDED or
   PROFILE_LINKED.

   If a profile is linked, the path of the profile can be any fully qualified name (including a network path) that can be opened using the
   CreateFile function.






   BLENDFUNCTION

   The BLENDFUNCTION structure controls blending by specifying the blending functions for source and destination bitmaps. 

   typedef struct _BLENDFUNCTION {
     BYTE     BlendOp;
     BYTE     BlendFlags;
     BYTE     SourceConstantAlpha;
     BYTE     AlphaFormat;
   }BLENDFUNCTION, *PBLENDFUNCTION, *LPBLENDFUNCTION;

   Members 

   BlendOp 
        Specifies the source blend operation. Currently, the only source and destination blend operation that has been defined is
        AC_SRC_OVER. For details, see the following Remarks section. 
   BlendFlags 
        Must be zero. 
   SourceConstantAlpha 
        Specifies an alpha transparency value to be used on the entire source bitmap. The SourceConstantAlpha value is combined with any
        per-pixel alpha values in the source bitmap. If you set SourceConstantAlpha to 0, it is assumed that your image is transparent. Set
        the SourceConstantAlpha value to 255 (opaque) when you only want to use per-pixel alpha values. 
   AlphaFormat 
        This member controls the way the source and destination bitmaps are interpreted. AlphaFormat has the following value. 
         Value
                                                  Meaning
         AC_SRC_ALPHA 
                                                  This flag is set when the bitmap has an Alpha channel (that is, per-pixel alpha).
                                                  Note that the APIs use premultiplied alpha, which means that the red, green and
                                                  blue channel values in the bitmap must be premultiplied with the alpha channel
                                                  value. For example, if the alpha channel value is x, the red, green and blue
                                                  channels must be multiplied by x and divided by 0xff prior to the call.


   Remarks

   When the AlphaFormat parameter is AC_SRC_ALPHA, the source bitmap must be 32 bpp. If it is not, the AlphaBlend function will fail. 

   When the BlendOp parameter is AC_SRC_OVER , the source bitmap is placed over the destination bitmap based on the alpha values of the source
   pixels. 

   If the source bitmap has no per-pixel alpha value (that is, AC_SRC_ALPHA is not set), the SourceConstantAlpha value determines the blend of
   the source and destination bitmaps, as shown in the following table. Note that SCA is used for SourceConstantAlpha here. Also, SCA is
   divided by 255 because it has a value that ranges from 0 to 255.

   Dst.Red =   Src.Red   * (SCA/255.0) + Dst.Red   * (1.0 - (SCA/255.0))
   Dst.Green = Src.Green * (SCA/255.0) + Dst.Green * (1.0 - (SCA/255.0))
   Dst.Blue =  Src.Blue  * (SCA/255.0) + Dst.Blue  * (1.0 - (SCA/255.0))


   If the destination bitmap has an alpha channel, then the blend is as follows.

   Dst.Alpha = Src.Alpha * (SCA/255.0) + Dst.Alpha * (1.0 - (SCA/255.0))


   If the source bitmap does not use SourceConstantAlpha (that is, it equals 0xFF), the per-pixel alpha determines the blend of the source and
   destination bitmaps, as shown in the following table.

   Dst.Red =   Src.Red +   (1 - Src.Alpha) * Dst.Red
   Dst.Green = Src.Green + (1 - Src.Alpha) * Dst.Green
   Dst.Blue =  Src.Blue +  (1 - Src.Alpha) * Dst.Blue


   If the destination bitmap has an alpha channel, then the blend is as follows.

   Dest.alpha = Src.Alpha + (1 - SrcAlpha) * Dst.Alpha


   If the source has both the SourceConstantAlpha (that is, it is not 0xFF) and per-pixel alpha, the source is pre-multiplied by the
   SourceConstantAlpha and then the blend is based on the per-pixel alpha. The following tables show this. Note that SourceConstantAlpha is
   divided by 255 because it has a value that ranges from 0 to 255.

   Src.Red = Src.Red * SourceConstantAlpha / 255.0;
   Src.Green = Src.Green * SourceConstantAlpha / 255.0;
   Src.Blue = Src.Blue * SourceConstantAlpha / 255.0;
   Src.Alpha = Src.Alpha * SourceConstantAlpha / 255.0;
   Dst.Red = Src.Red + (1 - Src.Alpha) * Dst.Red
   Dst.Green = Src.Green + (1 - Src.Alpha) * Dst.Green
   Dst.Blue = Src.Blue + (1 - Src.Alpha) * Dst.Blue
   Dst.Alpha = Src.Alpha + (1 - Src.Alpha) * Dst.Alpha






   COLORADJUSTMENT

   The COLORADJUSTMENT structure defines the color adjustment values used by the StretchBlt and StretchDIBits functions when the stretch mode
   is HALFTONE. You can set the color adjustment values by calling the SetColorAdjustment function. 

   typedef struct  tagCOLORADJUSTMENT {
     WORD  caSize; 
     WORD  caFlags; 
     WORD  caIlluminantIndex; 
     WORD  caRedGamma; 
     WORD  caGreenGamma; 
     WORD  caBlueGamma; 
     WORD  caReferenceBlack; 
     WORD  caReferenceWhite; 
     SHORT caContrast; 
     SHORT caBrightness; 
     SHORT caColorfulness; 
     SHORT caRedGreenTint; 
   } COLORADJUSTMENT, *PCOLORADJUSTMENT; 

   Members

   caSize 
        Specifies the size, in bytes, of the structure. 
   caFlags 
        Specifies how the output image should be prepared. This member may be set to NULL or any combination of the following values. 
         Value
                                                Meaning
         CA_NEGATIVE
                                                Specifies that the negative of the original image should be displayed.
         CA_LOG_FILTER
                                                Specifies that a logarithmic function should be applied to the final density of the
                                                output colors. This will increase the color contrast when the luminance is low.


   caIlluminantIndex 
        Specifies the type of standard light source under which the image is viewed. This member may be set to one of the following values. 
         Value
                               Meaning
         ILLUMINANT_DEVICE_DEFAULT
                               The device default. Standard used by output devices.
         ILLUMINANT_A
                               Tungsten lamp.
         ILLUMINANT_B
                               Noon sunlight.
         ILLUMINANT_C
                               NTSC daylight.
         ILLUMINANT_D50
                               Normal print.
         ILLUMINANT_D55
                               Bond paper print.
         ILLUMINANT_D65
                               Standard daylight. Standard for CRTs and pictures.
         ILLUMINANT_D75
                               Northern daylight.
         ILLUMINANT_F2
                               Cool white lamp.
         ILLUMINANT_TUNGSTEN
                               Same as ILLUMINANT_A.
         ILLUMINANT_DAYLIGHT
                               Same as ILLUMINANT_C.
         ILLUMINANT_FLUORESCENT
                               Same as ILLUMINANT_F2.
         ILLUMINANT_NTSC
                               Same as ILLUMINANT_C.


   caRedGamma 
        Specifies the nth power gamma-correction value for the red primary of the source colors. The value must be in the range from 2500 to
        65,000. A value of 10,000 means no gamma correction. 
   caGreenGamma 
        Specifies the nth power gamma-correction value for the green primary of the source colors. The value must be in the range from 2500
        to 65,000. A value of 10,000 means no gamma correction. 
   caBlueGamma 
        Specifies the nth power gamma-correction value for the blue primary of the source colors. The value must be in the range from 2500 to
        65,000. A value of 10,000 means no gamma correction. 
   caReferenceBlack 
        Specifies the black reference for the source colors. Any colors that are darker than this are treated as black. The value must be in
        the range from 0 to 4000. 
   caReferenceWhite 
        Specifies the white reference for the source colors. Any colors that are lighter than this are treated as white. The value must be in
        the range from 6000 to 10,000. 
   caContrast 
        Specifies the amount of contrast to be applied to the source object. The value must be in the range from ?100 to 100. A value of 0
        means no contrast adjustment. 
   caBrightness 
        Specifies the amount of brightness to be applied to the source object. The value must be in the range from ?100 to 100. A value of 0
        means no brightness adjustment. 
   caColorfulness 
        Specifies the amount of colorfulness to be applied to the source object. The value must be in the range from ?100 to 100. A value of
        0 means no colorfulness adjustment. 
   caRedGreenTint 
        Specifies the amount of red or green tint adjustment to be applied to the source object. The value must be in the range from ?100 to
        100. Positive numbers adjust towards red and negative numbers adjust towards green. Zero means no tint adjustment. 







*/

#endif



class moImageFile_BMP : public moImageFile
{


// NOTE: in order to have everything properly aligned
// 	 in the header we read the magic seperatly

#define	BMP_MAGIC(a, b)		(((a)<<8)|(b))

struct bmp_file_header_t {
	// char		magic[2];	-- this is read seperatly so the structure is aligned
	unsigned long	file_size;	// total size of this bitmap
	long		unused;		// set to 0 when saving
	unsigned long	offset;		// where the image data really is
	unsigned long	info_size;	// this is actually part of the bmp_info_header...
};


struct bmp_dir_header_t {
	long		file_size;	// size of the file including the dir entry
	long		end;		// position in the file after this entry
	long		offset;		// offset to these data
};

struct bmp_info_header_t {
	// the size in read in the file header instead
	long		width;
	long		height;
	short		planes;
	short		bits_per_pixel;
	unsigned long	compression;
	unsigned long	image_size;
	long		x_pixels_per_meter;
	long		y_pixels_per_meter;
	unsigned long	number_colors;
	unsigned long	colors_important;
	unsigned long	mask[4];		// red, green, blue, alpha
	unsigned long	color_space;
	unsigned long	red_primary_x;
	unsigned long	red_primary_y;
	unsigned long	red_primary_z;
	unsigned long	green_primary_x;
	unsigned long	green_primary_y;
	unsigned long	green_primary_z;
	unsigned long	blue_primary_x;
	unsigned long	blue_primary_y;
	unsigned long	blue_primary_z;
	unsigned long	gamma_scale_red;
	unsigned long	gamma_scale_green;
	unsigned long	gamma_scale_blue;
	unsigned long	itent;
	unsigned long	profile_data;
	unsigned long	profile_size;
	unsigned long	reserved;

// the following are added by us to fasten the loading & saving processes
	unsigned char	rshift[4];	/* Flawfinder: ignore */
	unsigned char	lshift[4];	/* Flawfinder: ignore */
	long		x;
	long		y;
	bool		done;		// in RLE we may get an EOI signal
};
 






typedef bool (moImageFile_BMP::*load_func_t)(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size);


bool LoadBW(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const signed char	*s;
	signed char		c;
	long			size, idx;

	size = (width + 7) / 8;
	if(file.Read(d, size) != size) {
		return false;
	}

	s = reinterpret_cast<const signed char *>(d) + size - 1;
	d += width;

	idx = width & 7;
	if(idx == 0) {
		idx = 8;
	}
	c = *s;
	c >>= (8 - idx) & 7;

	for(;;) {
		width -= idx;
		d -= idx;
		do {
			idx--;
			d[idx] = palette[c & 1];
			c >>= 1;
		} while(idx > 0);
		if(width == 0) {
			return true;
		}
		s--;
		c = *s;
		idx = 8;
	}
	/*NOTREACHED*/
}


bool LoadG4(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const unsigned char	*s;
	unsigned long		size, idx;

	size = (width + 1) / 2;
	if(file.Read(d, size) != static_cast<int>(size)) {
		return false;
	}

	s = reinterpret_cast<const unsigned char *>(d) + size;
	d += width;

	if((width & 1) != 0) {
		d--;
		s--;
		idx = *s >> 4;
		if(idx < palette_size) {
			*d = palette[idx];
		}
		width--;
	}

	while(width > 0) {
		d -= 2;
		s--;
		idx = *s;
		if((idx >> 4) < palette_size) {
			d[0] = palette[idx >> 4];
		}
		idx &= 15;
		if(idx < palette_size) {
			d[1] = palette[idx];
		}
		width -= 2;
	}

	return true;
}


bool LoadG4_RLE(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	unsigned char	*s, rle[2];		/* Flawfinder: ignore */
	unsigned long	size, idx1, idx2;
	moRGBA		*e;

	info.y--;
	d += info.x;
	for(;;) {
		if(file.Read(rle, 2) != 2) {
			return false;
		}
		if(rle[0] == 0) switch(rle[1]) {
		case 0:			// end of line, just get to the next line
			info.x = 0;
			return true;

		case 1:
			info.done = true;
			return true;

		case 2:
			if(file.Read(rle, 2) != 2) {
				return false;
			}
			info.x += rle[0];
			info.y -= rle[1] - 1;
			// if the following is false then the coordinates are outside the
			// bitmap - thus it's an error and we can't go on
			return info.x >= info.width || info.y < 0;

		default:
			info.x += rle[1];
			if(info.x > info.width) {
				// out of bitmap - IT CAN HAPPEN WITH ODD SIZES!!!
				width = info.width - info.x + rle[1];
				if(width <= 0) {
					return false;
				}
				rle[1] = static_cast<unsigned char>(width);
			}
			size = ((rle[1] + 3) / 2) & -2;
			if(file.Read(d, size) != static_cast<int>(size)) {
				// I/O error
				return false;
			}
			size = rle[1];
			s = reinterpret_cast<unsigned char *>(d) + size / 2;
			e = d += size;
			if((size & 1) != 0) {
				e--;
				idx1 = *s >> 4;
				if(idx1 < palette_size) {
					*e = palette[idx1];
				}
				size--;
			}
			while(size > 0) {
				size -= 2;
				e -= 2;
				s--;
				idx1 = *s & 15;
				if(idx1 < palette_size) {
					e[1] = palette[idx1];
				}
				idx2 = *s >> 4;
				if(idx2 < palette_size) {
					e[0] = palette[idx2];
				}
			}
			break;

		}
		else {
			size = rle[0];
			info.x += size;
			if(info.x > info.width) {
				// out of bitmap - IT CAN HAPPEN WITH ODD SIZES!!!
				size = info.width - info.x + size;
				if(static_cast<long>(size) <= 0) {
					return false;
				}
			}
			idx1 = rle[1] >> 4;
			idx2 = rle[1] & 15;
			if(idx1 < palette_size || idx2 < palette_size) {
				if((size & 1) != 0) {
					if(idx1 < palette_size) {
						*d = palette[idx1];
					}
					d++;
					size--;
				}
				while(size > 0) {
					size -= 2;
					if(idx1 < palette_size) {
						d[0] = palette[idx1];
					}
					if(idx2 < palette_size) {
						d[1] = palette[idx2];
					}
					d += 2;
				}
			}
			else {
				d += size;
			}
		}
	}
	/*NOTREACHED*/
}


bool LoadG8(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const unsigned char	*s;
	unsigned long		idx;

	if(file.Read(d, width) != width) {
		return false;
	}

	s = reinterpret_cast<const unsigned char *>(d) + width;
	d += width;

	do {
		d--;
		s--;
		idx = *s;
		if(idx < palette_size) {
			*d = palette[idx];
		}
		width--;
	} while(width > 0);

	return true;
}


bool LoadG8_RLE(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const unsigned char	*s;
	unsigned char		rle[2];			/* Flawfinder: ignore */
	unsigned long		size, idx;
	moRGBA			*e;

	info.y--;
	d += info.x;
	width -= info.x;
	for(;;) {
		if(file.Read(rle, 2) != 2) {
			return false;
		}

		if(rle[0] == 0) switch(rle[1]) {
		case 0:			// end of line, just get to the next line
			info.x = 0;
			return true;

		case 1:
			info.done = true;
			return true;

		case 2:
			if(file.Read(rle, 2) != 2) {
				return false;
			}
			info.x += rle[0];
			info.y -= rle[1] - 1;
			// if the following is false then the coordinates are outside the
			// bitmap - thus it's an error and we can't go on
			return info.x >= info.width || info.y < 0;

		default:
			info.x += rle[1];
			if(info.x > info.width) {
				// out of bitmap
				return false;
			}
			size = (rle[1] + 1) & -2;
			if(file.Read(d, size) != static_cast<int>(size)) {
				// I/O error
				return false;
			}
			size = rle[1];
			s = reinterpret_cast<const unsigned char *>(d) + size;
			e = d += size;
			do {
				size--;
				e--;
				s--;
				idx = *s;
				if(idx < palette_size) {
					*e = palette[idx];
				}
			} while(size > 0);
			break;

		}
		else {
			info.x += rle[0];
			if(info.x > info.width) {
				// out of bitmap
				return false;
			}
			size = rle[0];
			idx = rle[1];
			if(idx < palette_size) {
				do {
					*d = palette[idx];
					d++;
					size--;
				} while(size > 0);
			}
			else {
				d += size;
			}
		}
	}
	/*NOTREACHED*/
}


bool LoadRGB555(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const unsigned short	*s;
	unsigned long		size, idx;

	size = width * 2;
	if(file.Read(d, size) != static_cast<int>(size)) {
		return false;
	}

	s = reinterpret_cast<const unsigned short *>(d) + width;
	d += width;
	do {
		d--;
		s--;
		idx = moReadLE16Bits(s);
		if(palette != 0) {
			if(idx < palette_size) {
				*d = palette[idx];
			}
		}
		else {
			d->red =   mo5BitsTo8Bits[(idx >> 10) & 31];
			d->green = mo5BitsTo8Bits[(idx >> 5) & 31];
			d->blue =  mo5BitsTo8Bits[idx & 31];
			d->alpha = 255;
		}
		width--;
	} while(width > 0);

	return true;
}


bool LoadRGB555Mask(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	unsigned short		*s;
	long			size;
	unsigned long		idx;

	size = width * 2;
	if(file.Read(d, size) != size) {
		return false;
	}

	s = reinterpret_cast<unsigned short *>(d) + width;
	d += width;
	do {
		d--;
		s--;
		idx = moReadLE16Bits(s);
		if(palette != 0) {
			if(idx < palette_size) {
				*d = palette[idx];
			}
		}
		else {
			d->red =   static_cast<unsigned char>(((idx & info.mask[0]) >> info.rshift[0]) << info.lshift[0]);
			d->green = static_cast<unsigned char>(((idx & info.mask[1]) >> info.rshift[1]) << info.lshift[1]);
			d->blue =  static_cast<unsigned char>(((idx & info.mask[2]) >> info.rshift[2]) << info.lshift[2]);
			if(info.mask[3] != 0) {
				d->alpha = static_cast<unsigned char>(((idx & info.mask[3]) >> info.rshift[3]) << info.lshift[3]);
			}
			else {
				d->alpha = 255;
			}
		}
		width--;
	} while(width > 0);

	return true;
}


bool LoadRGB(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	const unsigned char	*s;
	unsigned char		r;
	long			size;

	size = width * 3;
	if(file.Read(d, size) != size) {
		return false;
	}

	s = reinterpret_cast<const unsigned char *>(d) + size;
	d += width;
	do {
		s -= 3;
		d--;
		r = s[2];		// the first pixel needs safe swapping
		d->blue =  s[0];
		d->green = s[1];
		d->red =   r;
		d->alpha = 255;		// no alpha...
		width--;
	} while(width > 0);

	return true;
}


bool LoadRGBA(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	unsigned char	r, alpha;
	long		size;

	size = width * 4;
	if(file.Read(d, size) != size) {
		return false;
	}

	// swap the red and blue & test the alpha channel
	alpha = 0;
	do {
		r = d->blue;		// the first pixel needs safe swapping
		d->blue = d->red;
		d->red = r;
		alpha |= d->alpha;
		d++;
		width--;
	} while(width > 0);

// sounds stupid? well... looks like many 32 bits images only use 0 as alpha!
	if(alpha == 0) {
		// NOTE: if all the data isn't in the cache at least the end is, so the following will have a fast start!
		while(size > 0) {
			size -= 4;
			d--;
			d->alpha = 255;
		}
	}

	return true;
}


bool LoadRGBAMask(moIStream& file, bmp_info_header_t& info, moRGBA *d, long width, const moRGBA *palette, unsigned long palette_size)
{
	unsigned long	bitfield;
	long		size;

	size = width * 4;
	if(file.Read(d, size) != size) {
		return false;
	}

	// swap the red and blue
	do {
		bitfield = moReadLE32Bits(d);
		d->red =   static_cast<unsigned char>(((bitfield & info.mask[0]) >> info.rshift[0]) << info.lshift[0]);
		d->green = static_cast<unsigned char>(((bitfield & info.mask[1]) >> info.rshift[1]) << info.lshift[1]);
		d->blue =  static_cast<unsigned char>(((bitfield & info.mask[2]) >> info.rshift[2]) << info.lshift[2]);
		if(info.mask[3] == 0) {		// any alpha mask?
			d->alpha = 255;
		}
		else {
			d->alpha = static_cast<unsigned char>(((bitfield & info.mask[3]) >> info.rshift[3]) << info.lshift[2]);
		}
		d++;
		width--;
	} while(width > 0);

	return true;
}


bool Load(moIStream& file, moImage& im)
{
	char			magic[2];		/* Flawfinder: ignore */
	unsigned short		magik;
	bool			result;
	bmp_file_header_t	header;
	bmp_info_header_t	info;
	bmp_dir_header_t	dir;
	load_func_t		load_image, load_palette;
	std::auto_ptr<moRGBA>	palette;
	moRGBA			*d;
	long			pos, x, y;
	unsigned long		mask;

// read the magic only
repeat:
	if(file.Read(magic, 2) != 2) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

// there are different possible magics
	magik = BMP_MAGIC(magic[0], magic[1]);
	switch(magik) {
	case BMP_MAGIC('B', 'A'):
		// we are only skipping these info at this time
		// (bitmap attributes?)
		if(file.Read(&dir, sizeof(dir)) != sizeof(dir)) {
			im.LastErrno(MO_ERROR_IO);
			return false;
		}
		goto repeat;

	case BMP_MAGIC('B', 'M'):
	case BMP_MAGIC('C', 'I'):
		break;

	default:
		im.LastErrno(MO_ERROR_INVALID);
		return false;

	}

// read the main header
	if(file.Read(&header, sizeof(header)) != sizeof(header)) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

#if BYTE_ORDER == BIG_ENDIAN
	header.offset =    moSwap32Bits((uint32_t) header.offset);
	header.info_size = moSwap32Bits((uint32_t) header.info_size);
#endif

	if(header.info_size < 12 || header.info_size > sizeof(info)) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	memset(&info, 0, sizeof(info));		// just in case we have a strange size we want the rest to be zeroes

// read the info now
	if(file.Read(&info, header.info_size - 4) != static_cast<int>(header.info_size - 4)) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

// make sure we've got the right info!
	if(header.info_size == 12) {
		// ha! this is actually an OS/2 bitmap
		// rearrange the structure properly
#if BYTE_ORDER == BIG_ENDIAN
		info.planes = static_cast<unsigned short>(info.height >> 16);
		info.bits_per_pixel = static_cast<unsigned short>(info.height);
		info.height = info.width;
		info.width = info.width >> 16;
#else
		info.planes = static_cast<unsigned short>(info.height);
		info.bits_per_pixel = static_cast<unsigned short>(info.height >> 16);
		info.height = info.width >> 16;
		info.width &= 0x0FFFF;
#endif
		load_palette = &moImageFile_BMP::LoadRGB;
	}
	else {
#if BYTE_ORDER == BIG_ENDIAN
		info.width = moSwap32Bits((int32_t) info.width);
		info.height = moSwap32Bits((int32_t) info.height);
		info.planes = moSwap16Bits(info.planes);
		info.bits_per_pixel = moSwap16Bits(info.bits_per_pixel);
		info.compression = moSwap32Bits((uint32_t) info.compression);
		info.number_colors = moSwap32Bits((uint32_t) info.number_colors);
#endif
		load_palette = &moImageFile_BMP::LoadRGBA;	// though there is not alpha, it looks like there is!
	}

// dunno why there could be more than one plane! (nor less)
	if(info.planes != 1 || static_cast<unsigned long>(info.compression) > 3
	|| static_cast<unsigned long>(info.number_colors) > 65536) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

// check out the compression & depth at once
#define	BMP_DEPTHCOMP(comp, bpp)	((bpp) | ((comp)<<16))
	switch(BMP_DEPTHCOMP(info.compression, info.bits_per_pixel)) {
	case BMP_DEPTHCOMP(0, 1):
		load_image = &moImageFile_BMP::LoadBW;
		break;

	case BMP_DEPTHCOMP(0, 4):
		load_image = &moImageFile_BMP::LoadG4;
		break;

	case BMP_DEPTHCOMP(0, 8):
		load_image = &moImageFile_BMP::LoadG8;
		break;

	case BMP_DEPTHCOMP(0, 15):	// make 15 == 16
		info.bits_per_pixel = 16;
	case BMP_DEPTHCOMP(0, 16):
		load_image = &moImageFile_BMP::LoadRGB555;
		break;

	case BMP_DEPTHCOMP(0, 24):
		load_image = &moImageFile_BMP::LoadRGB;
		break;

	case BMP_DEPTHCOMP(0, 32):
		load_image = &moImageFile_BMP::LoadRGBA;
		break;

	case BMP_DEPTHCOMP(1, 8):
		load_image = &moImageFile_BMP::LoadG8_RLE;
		break;

	case BMP_DEPTHCOMP(2, 4):
		load_image = &moImageFile_BMP::LoadG4_RLE;
		break;

	case BMP_DEPTHCOMP(3, 15):	// make 15 == 16
		info.bits_per_pixel = 16;
	case BMP_DEPTHCOMP(3, 16):
		load_image = &moImageFile_BMP::LoadRGB555Mask;
		break;

	case BMP_DEPTHCOMP(3, 32):
		load_image = &moImageFile_BMP::LoadRGBAMask;
		break;

	default:
		im.LastErrno(MO_ERROR_INVALID);
		return false;

	}


// in case of MASK compression we need to read the said masks
	if(info.compression == 3) {
		if(header.info_size <= 40) {
			// doesn't look good... we have to load extra data which should be in the header!
			if(file.Read(&info.mask, 12) != 12) {
				return false;
			}
		}
		if(header.info_size < 52) {		// any alpha specified? (V3 vs V4)
			info.mask[3] = 0;
		}
		for(pos = 0; pos < 4; pos++) {
#if BYTE_ORDER == BIG_ENDIAN
			info.mask[pos] = moSwap32Bits((uint32_t) info.mask[pos]);
#endif
			mask = info.mask[pos];
			if(mask == 0) {
				info.rshift[pos] = 0;
				info.lshift[pos] = 0;
			}
			else {
				x = info.bits_per_pixel;
				for(y = 0; y < x; y++) {
					if((mask & 1) != 0) {
						info.rshift[pos] = static_cast<unsigned char>(y);
						break;
					}
					mask >>= 1;
				}
				y = 0;
				while(y < 8 && (signed char) mask > 0) {
					mask <<= 1;
					y++;
				}
				info.lshift[pos] = static_cast<unsigned char>(y);
			}
		}
	}

// If there is a colormap, load it now
	if(info.number_colors != 0 || info.bits_per_pixel < 16) {
		if(info.number_colors == 0) {
			// default number of color depends on the depth in this case
			info.number_colors = 1 << info.bits_per_pixel;
		}
		palette.reset(new moRGBA[info.number_colors]);
		(this->*load_palette)(file, info, palette.get(), info.number_colors, 0, 0);
	}

// now we can load the bitmap
	result = true;		// this is important when compressed images are dealt with
	y = labs(info.height);
	im.SetSize(info.width, y);
	d = im.Data();

	if(info.number_colors == 1) {
		// a special case with a palette of 1 entry
		// note that we don't even check the data!!!
		y *= im.Width();
		do {
			*d = *palette;
			d++;
			y--;
		} while(y > 0);
		return true;
	}

	file.ReadPosition(header.offset);
	x = im.Width();
	if(info.height < 0) {
		info.height = y;
		d += im.Size();
		if(info.compression == 1 || info.compression == 2) {
			info.y = y - 1;
			do {
				y--;
				d -= x;
				if(info.y == y) {
					result = (this->*load_image)(file, info, d, x, palette.get(), info.number_colors);
				}
				/* adjust the position to the next 4 bytes - not sure why the creator though of such a neat feature */
				//pos = (((file.ReadPosition() - header.offset) + 3) & -4) + header.offset;
				//file.SetReadPosition(pos);
			} while(y > 0 && result && !info.done);
		}
		else {
			do {
				d -= x;
				result = (this->*load_image)(file, info, d, x, palette.get(), info.number_colors);
				/* adjust the position to the next 4 bytes - not sure why the creator though of such a neat feature */
				pos = static_cast<long>((((file.ReadPosition() - header.offset) + 3) & -4) + header.offset);
				file.ReadPosition(pos);
				y--;
			} while(y > 0 && result);
		}
	}
	else {
		if(info.compression == 1 || info.compression == 2) {
			info.y = y - 1;
			do {
				y--;
				if(info.y == y) {
					result = (this->*load_image)(file, info, d, x, palette.get(), info.number_colors);
				}
				/* adjust the position to the next 4 bytes - not sure why the creator though of such a neat feature */
				//pos = (((file.ReadPosition() - header.offset) + 3) & -4) + header.offset;
				//file.SetReadPosition(pos);
				d += x;
			} while(y > 0 && result && !info.done);
		}
		else {
			do {
				result = (this->*load_image)(file, info, d, x, palette.get(), info.number_colors);
				/* adjust the position to the next 4 bytes - not sure why the creator though of such a neat feature */
				pos = static_cast<long>((((file.ReadPosition() - header.offset) + 3) & -4) + header.offset);
				file.ReadPosition(pos);
				d += x;
				y--;
			} while(y > 0 && result);
		}
	}

	return result;
}






typedef void (moImageFile_BMP::*save_func_t)(unsigned char *s, const moRGBA *d, long width);


void SaveGrey(unsigned char *s, const moRGBA *d, long width)
{
	do {
		*s = d->Grey();
		s++;
		d++;
		width--;
	} while(width > 0);
}



void SaveRGB555(unsigned char *s, const moRGBA *d, long width)
{
	long		l;

	do {
		l = ((d->red & 0xF8) << 7) + ((d->green & 0xF8) << 2) + (d->blue >> 3);
		s[0] = static_cast<unsigned char>(l);
		s[1] = static_cast<unsigned char>(l >> 8);
		s += 2;
		d++;
		width--;
	} while(width > 0);
}



void SaveRGB(unsigned char *s, const moRGBA *d, long width)
{
	do {
		s[0] = d->blue;
		s[1] = d->green;
		s[2] = d->red;
		s += 3;
		d++;
		width--;
	} while(width > 0);
}



void SaveRGBA(unsigned char *s, const moRGBA *d, long width)
{
	do {
		s[0] = d->blue;
		s[1] = d->green;
		s[2] = d->red;
		s[3] = d->alpha;
		s += 4;
		d++;
		width--;
	} while(width > 0);
}


void SaveRGBX(unsigned char *s, const moRGBA *d, long width)
{
	do {
		s[0] = d->blue;
		s[1] = d->green;
		s[2] = d->red;
		// s[3] is and remains 0
		s += 4;
		d++;
		width--;
	} while(width > 0);
}




bool Save(moOStream& file, const moImage& im)
{
	char				magic[2];			/* Flawfinder: ignore */
	bmp_file_header_t		header;
	bmp_info_header_t		info;
	long				y, row_size, palette_size;
	save_func_t			save_func;
	const moRGBA			*d;
	std::auto_ptr<unsigned char>	row;
	moRGBA				*palette;

	if(im.IsEmpty()) {
		return false;
	}


	info.width = im.Width();
	info.height = im.Height();
	info.planes = 1;
	info.compression = 0;
	info.x_pixels_per_meter = 7874;		/* 200x200 dpi... */
	info.y_pixels_per_meter = 7874;
	info.number_colors = 0;
	info.colors_important = 0;

	/* DEPTH */
	palette_size = 0;
	switch(DefineDepth(im, true)) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		info.bits_per_pixel = 8;
		save_func = &moImageFile_BMP::SaveGrey;
		palette_size = 256 * 4;
		break;

	case MO_IMAGE_FILE_DEPTH_RGB555:
		info.bits_per_pixel = 16;
		save_func = &moImageFile_BMP::SaveRGB555;
		break;

	default:		// case MO_IMAGE_FILE_DEPTH_RGB:
		info.bits_per_pixel = 24;
		save_func = &moImageFile_BMP::SaveRGB;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBX:
		info.bits_per_pixel = 32;
		save_func = &moImageFile_BMP::SaveRGBX;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBA:
		info.bits_per_pixel = 32;
		if(im.IsSolid()) {
			save_func = &moImageFile_BMP::SaveRGBX;
		}
		else {
			save_func = &moImageFile_BMP::SaveRGBA;
		}
		break;

	}

	row_size = ((info.width * info.bits_per_pixel / 8 + 3) & -4);
	info.image_size = row_size * info.height;

	// we need a temporary row to convert the data
	row.reset(new unsigned char[row_size > palette_size ? row_size : palette_size]);

	magic[0] = 'B';
	magic[1] = 'M';
	if(file.Write(magic, 2) != 2) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	header.file_size = 2 + sizeof(header) + 36 + info.image_size + palette_size;
	header.unused = 0;
	header.offset = 2 + sizeof(header) + 36 + palette_size;
	header.info_size = 40;
	if(file.Write(&header, sizeof(header)) != sizeof(header)) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	if(file.Write(&info, 36) != 36) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	if(palette_size != 0) {
		// we use the row as a temporary buffer
		palette = reinterpret_cast<moRGBA *>(row.get()) + 256;
		y = 256;
		while(y > 0) {
			y--;
			palette--;
			palette->red   = static_cast<unsigned char>(y);
			palette->green = static_cast<unsigned char>(y);
			palette->blue  = static_cast<unsigned char>(y);
			palette->alpha = 0;		// this is the usual for a palette, go figure
		}
		if(file.Write(row.get(), 256 * 4) != 256 * 4) {
			im.LastErrno(MO_ERROR_IO);
			return false;
		}
	}

	memset(row.get(), 0, row_size);		// clear the end and the padding in RGBX
	y = info.height;
	d = im.Data();
	while(y > 0) {
		y--;
		(this->*save_func)(row.get(), d, info.width);
		if(file.Write(row.get(), row_size) != row_size) {
			im.LastErrno(MO_ERROR_IO);
			return false;
		}
		d += info.width;
	}

	return true;
}





virtual const char *MimeType(void) const
{
	return "image/bmp";
}

virtual long MatchPriority(void) const
{
	return 95;		// pretty good - but test after some other major formats
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
	const char *s = reinterpret_cast<const char *>(buffer);

	if(size < 2) {
		return 0;
	}

	switch(BMP_MAGIC(s[0], s[1])) {
	case BMP_MAGIC('B', 'A'):
	case BMP_MAGIC('B', 'M'):
	case BMP_MAGIC('C', 'I'):
		break;

	default:
		return 0;

	}

	// TODO: add some code in order to test the header some

	return 100;
}



};		// class moImageFile_BMP


MAKE_IMAGEFILE_PUBLIC(BMP);


}			// namespace molib;

// vim: ts=8

