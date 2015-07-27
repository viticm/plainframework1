# This file generate by plain framework admin tool, date: 2015-05-11 19:17:24
SET(LIBRARY_OUTPUT_PATH ./../../../dll/)


SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./../../../dependencies/dll/"
  "./../../../core/dll/"
)

INCLUDE_DIRECTORIES(
  "../include/"
  "../misc/"
  "../../../dependencies/lua/include/"
  "../../../dependencies/iconv/include/"
  "../../../dependencies/zlib/pf_project/include/"
  "../../../core/include/"
)

SET (MISC_LIST
  ../misc/crc32.h
  ../misc/md5.h
)

SET (MISC_BZIP2_LIST
  ../misc/bzip2/bzlib.h
  ../misc/bzip2/bzlib_private.h
)

SET (MISC_HUFFMAN_LIST
  ../misc/huffman/huff.h
)

SET (MISC_PKLIB_LIST
  ../misc/pklib/pklib.h
)

SET (MISC_WAVE_LIST
  ../misc/wave/wave.h
)
SET (INCLUDE_PAK_LIST
  ../include/pak/compress.h
  ../include/pak/config.h
  ../include/pak/encrypt.h
  ../include/pak/file.h
  ../include/pak/interface.h
  ../include/pak/listfile.h
  ../include/pak/util.h
)
SET (MISC_LIST
  ../misc/crc32.cpp
  ../misc/md5.cpp
)

SET (MISC_BZIP2_LIST
  ../misc/bzip2/blocksort.c
  ../misc/bzip2/bzlib.c
  ../misc/bzip2/compress.c
  ../misc/bzip2/crctable.c
  ../misc/bzip2/decompress.c
  ../misc/bzip2/huffman.c
  ../misc/bzip2/randtable.c
)

SET (MISC_HUFFMAN_LIST
  ../misc/huffman/huff.cpp
)

SET (MISC_PKLIB_LIST
  ../misc/pklib/crc32.c
  ../misc/pklib/explode.c
  ../misc/pklib/implode.c
)

SET (MISC_WAVE_LIST
  ../misc/wave/wave.cpp
)
SET (SRC_LIST
  ../src/compress.cc
  ../src/encrypt.cc
  ../src/file.cc
  ../src/interface.cc
  ../src/listfile.cc
  ../src/util.cc
)


ADD_LIBRARY(plugin_pak SHARED
  ${SRC_LIST}
  ${MISC_LIST}
  ${MISC_BZIP2_LIST}
  ${MISC_HUFFMAN_LIST}
  ${MISC_PKLIB_LIST}
  ${MISC_WAVE_LIST}
)

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(plugin_pak ws2_32.lib pf_core.lib lua.lib zlib.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(plugin_pak pf_core lua zlib iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
