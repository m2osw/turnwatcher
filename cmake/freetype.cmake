FIND_PATH(FREETYPE_INCLUDE_DIR_ft2build ft2build.h 
  HINTS
  $ENV{FREETYPE_DIR}
  PATH_SUFFIXES include
  PATHS
  /usr/include/freetype2
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(FREETYPE_INCLUDE_DIR_freetype2 freetype/config/ftheader.h 
  HINTS
  $ENV{FREETYPE_DIR}/include/freetype2
  PATHS
  /usr/include/freetype2
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
  PATH_SUFFIXES freetype2
)

FIND_LIBRARY( FREETYPE_LIBRARY
  NAMES freetype libfreetype freetype219
  HINTS
  $ENV{FREETYPE_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/lib/i386-linux-gnu/
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

