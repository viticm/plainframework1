cmake_minimum_required(VERSION 2.6)

SET(LIBRARY_OUTPUT_PATH "../dll")

##update_begin

SET (SOURCEFILES_LIST
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)

SET (HEADERFILES_LIST
	crc32.h
	deflate.h
	gzguts.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zconf.h
	zlib.h
	zutil.h
)



if (WIN32)
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
endif()

ADD_LIBRARY(zlib SHARED
	${SOURCEFILES_LIST}
	${HEADERFILES_LIST}
)


##update_end

IF(CMAKE_SYSTEM MATCHES Windows)

ELSE()
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
  endif(USE_32BITS)
ENDIF(CMAKE_SYSTEM MATCHES Windows)
