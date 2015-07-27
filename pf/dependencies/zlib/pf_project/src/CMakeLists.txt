# This file generate by plain framework admin tool, date: 2015-05-09 13:21:49
SET(LIBRARY_OUTPUT_PATH ./../../../dll/)


SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./"
)

INCLUDE_DIRECTORIES(
  "../include/"
)


SET (INCLUDE_LIST
  ../include/deflate.h
  ../include/gzguts.h
  ../include/zconf.h
  ../include/zlib.h
  ../include/zutil.h
)

SET (SRC_LIST
  ../src/adler32.c
  ../src/compress.c
  ../src/crc32.c
  ../src/deflate.c
  ../src/gzclose.c
  ../src/gzlib.c
  ../src/gzread.c
  ../src/gzwrite.c
  ../src/infback.c
  ../src/inffast.c
  ../src/inflate.c
  ../src/inftrees.c
  ../src/trees.c
  ../src/uncompr.c
  ../src/zutil.c
)


ADD_LIBRARY(zlib SHARED
  ${SRC_LIST}
)

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(zlib ws2_32.lib odbc32.lib odbccp32.lib )
else()
TARGET_LINK_LIBRARIES(zlib odbc )
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
