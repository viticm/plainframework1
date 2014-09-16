SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ADD_DEFINITIONS(-D_PF_SIMPLE)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../../pf/core/dll" #linux
  "../../../pf/dependencies/dll" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../pf/core/include",
                    "../include",
                    "../../../pf/dependencies/iconv/include",
                    "../../../pf/dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_LIST
	../src/main.cc
)

SET (HEADERFILES_LIST
	../include/main.h
)

SET (RESOURCEFILES_LIST
)



if (WIN32)
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
endif()


ADD_EXECUTABLE(pf_simple
	${SOURCEFILES_LIST}
	${HEADERFILES_LIST}
	${RESOURCEFILES_LIST}
)


##update_end

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(pf_simple ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib  lua.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(pf_simple pthread odbc pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
