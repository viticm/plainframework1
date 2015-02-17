SET(TF_CURRENT_BINARY_PATH "../dll/")

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../dependencies/dll" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../core/include",
                    "../include",
                    "../../dependencies/iconv/include",
                    "../../dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_UTIL_COMPRESSOR_LIST
	../src/util/compressor/assistant.cc
	../src/util/compressor/mini.cc
	../src/util/compressor/minilzo.c
	../src/util/compressor/minimanager.cc
)

SET (SOURCEFILES_UTIL_LIST
)

SET (SOURCEFILES_BASE_HASHMAP_LIST
	../src/base/hashmap/integer.cc
)

SET (SOURCEFILES_BASE_LIST
	../src/base/global.cc
	../src/base/io.cc
	../src/base/log.cc
	../src/base/md5.cc
	../src/base/string.cc
	../src/base/time_manager.cc
	../src/base/tinytimer.cc
	../src/base/util.cc
)

SET (SOURCEFILES_DB_ODBC_LIST
	../src/db/odbc/interface.cc
	../src/db/odbc/system.cc
)

SET (SOURCEFILES_DB_LIST
	../src/db/manager.cc
)

SET (SOURCEFILES_ENGINE_THREAD_LIST
	../src/engine/thread/db.cc
	../src/engine/thread/net.cc
	../src/engine/thread/performance.cc
	../src/engine/thread/script.cc
)

SET (SOURCEFILES_ENGINE_LIST
	../src/engine/kernel.cc
)

SET (SOURCEFILES_FILE_LIST
	../src/file/api.cc
	../src/file/database.cc
	../src/file/ini.cc
)

SET (SOURCEFILES_NET_PACKET_LIST
	../src/net/packet/base.cc
	../src/net/packet/factorymanager.cc
)

SET (SOURCEFILES_NET_SOCKET_LIST
	../src/net/socket/api.cc
	../src/net/socket/base.cc
	../src/net/socket/compressor.cc
	../src/net/socket/encode.cc
	../src/net/socket/encryptor.cc
	../src/net/socket/inputstream.cc
	../src/net/socket/outputstream.cc
	../src/net/socket/server.cc
	../src/net/socket/stream.cc
)

SET (SOURCEFILES_NET_CONNECTION_MANAGER_LIST
	../src/net/connection/manager/base.cc
	../src/net/connection/manager/epoll.cc
	../src/net/connection/manager/iocp.cc
	../src/net/connection/manager/select.cc
)

SET (SOURCEFILES_NET_CONNECTION_LIST
	../src/net/connection/base.cc
	../src/net/connection/pool.cc
)

SET (SOURCEFILES_NET_LIST
	../src/net/manager.cc
)

SET (SOURCEFILES_PERFORMANCE_LIST
	../src/performance/eyes.cc
)

SET (SOURCEFILES_SCRIPT_CACHE_LIST
	../src/script/cache/base.cc
	../src/script/cache/manager.cc
)

SET (SOURCEFILES_SCRIPT_LUA_LIST
	../src/script/lua/filebridge.cc
	../src/script/lua/interface.cc
	../src/script/lua/lunar.cc
	../src/script/lua/stack.cc
	../src/script/lua/system.cc
	../src/script/lua/vm.cc
)

SET (SOURCEFILES_SCRIPT_LIST
)

SET (SOURCEFILES_SYS_MEMORY_LIST
	../src/sys/memory/dynamic_allocator.cc
	../src/sys/memory/share.cc
	../src/sys/memory/static_allocator.cc
)

SET (SOURCEFILES_SYS_LIST
	../src/sys/assert.cc
	../src/sys/info.cc
	../src/sys/minidump.cc
	../src/sys/process.cc
	../src/sys/thread.cc
	../src/sys/util.cc
)

SET (SOURCEFILES_EVENT_LIST
	../src/event/system.cc
)

SET (SOURCEFILES_LIST
)

SET (HEADERFILES_UTIL_COMPRESSOR_LIST
	../include/pf/util/compressor/assistant.h
	../include/pf/util/compressor/config.h
	../include/pf/util/compressor/lzoconf.h
	../include/pf/util/compressor/lzodefs.h
	../include/pf/util/compressor/mini.h
	../include/pf/util/compressor/minilzo.h
	../include/pf/util/compressor/minimanager.h
)

SET (HEADERFILES_UTIL_LIST
	../include/pf/util/config.h
)

SET (HEADERFILES_BASE_HASHMAP_LIST
	../include/pf/base/hashmap/config.h
	../include/pf/base/hashmap/integer.h
	../include/pf/base/hashmap/template.h
)

SET (HEADERFILES_BASE_LIST
	../include/pf/base/base64.hpp
	../include/pf/base/config.h
	../include/pf/base/global.h
	../include/pf/base/io.h
	../include/pf/base/log.h
	../include/pf/base/md5.h
	../include/pf/base/singleton.h
	../include/pf/base/string.h
	../include/pf/base/time_manager.h
	../include/pf/base/tinytimer.h
	../include/pf/base/util.h
)

SET (HEADERFILES_DB_ODBC_LIST
	../include/pf/db/odbc/config.h
	../include/pf/db/odbc/interface.h
	../include/pf/db/odbc/system.h
)

SET (HEADERFILES_DB_LIST
	../include/pf/db/config.h
	../include/pf/db/manager.h
)

SET (HEADERFILES_ENGINE_THREAD_LIST
	../include/pf/engine/thread/config.h
	../include/pf/engine/thread/db.h
	../include/pf/engine/thread/net.h
	../include/pf/engine/thread/performance.h
	../include/pf/engine/thread/script.h
)

SET (HEADERFILES_ENGINE_LIST
	../include/pf/engine/config.h
	../include/pf/engine/kernel.h
)

SET (HEADERFILES_FILE_LIST
	../include/pf/file/api.h
	../include/pf/file/config.h
	../include/pf/file/database.h
	../include/pf/file/ini.h
)

SET (HEADERFILES_NET_SOCKET_LIST
	../include/pf/net/socket/api.h
	../include/pf/net/socket/base.h
	../include/pf/net/socket/compressor.h
	../include/pf/net/socket/config.h
	../include/pf/net/socket/encode.h
	../include/pf/net/socket/encryptor.h
	../include/pf/net/socket/extend.inl
	../include/pf/net/socket/inputstream.h
	../include/pf/net/socket/outputstream.h
	../include/pf/net/socket/server.h
	../include/pf/net/socket/stream.h
)

SET (HEADERFILES_NET_PACKET_LIST
	../include/pf/net/packet/base.h
	../include/pf/net/packet/config.h
	../include/pf/net/packet/factory.h
	../include/pf/net/packet/factorymanager.h
)

SET (HEADERFILES_NET_CONNECTION_MANAGER_LIST
	../include/pf/net/connection/manager/base.h
	../include/pf/net/connection/manager/config.h
	../include/pf/net/connection/manager/epoll.h
	../include/pf/net/connection/manager/iocp.h
	../include/pf/net/connection/manager/select.h
)

SET (HEADERFILES_NET_CONNECTION_LIST
	../include/pf/net/connection/base.h
	../include/pf/net/connection/config.h
	../include/pf/net/connection/pool.h
)

SET (HEADERFILES_NET_LIST
	../include/pf/net/config.h
	../include/pf/net/manager.h
)

SET (HEADERFILES_PERFORMANCE_LIST
	../include/pf/performance/config.h
	../include/pf/performance/eyes.h
)

SET (HEADERFILES_SCRIPT_CACHE_LIST
	../include/pf/script/cache/base.h
	../include/pf/script/cache/config.h
	../include/pf/script/cache/manager.h
)

SET (HEADERFILES_SCRIPT_LUA_LIST
	../include/pf/script/lua/config.h
	../include/pf/script/lua/filebridge.h
	../include/pf/script/lua/interface.h
	../include/pf/script/lua/lunar.h
	../include/pf/script/lua/stack.h
	../include/pf/script/lua/system.h
	../include/pf/script/lua/vm.h
)

SET (HEADERFILES_SCRIPT_LIST
	../include/pf/script/config.h
)

SET (HEADERFILES_SYS_MEMORY_LIST
	../include/pf/sys/memory/config.h
	../include/pf/sys/memory/dynamic_allocator.h
	../include/pf/sys/memory/share.h
	../include/pf/sys/memory/static_allocator.h
)

SET (HEADERFILES_SYS_LIST
	../include/pf/sys/assert.h
	../include/pf/sys/config.h
	../include/pf/sys/info.h
	../include/pf/sys/minidump.h
	../include/pf/sys/process.h
	../include/pf/sys/thread.h
	../include/pf/sys/util.h
)

SET (HEADERFILES_EVENT_LIST
	../include/pf/event/config.h
	../include/pf/event/system.h
)

SET (HEADERFILES_LIST
)



if (WIN32)
	source_group(SourceFiles\\util\\compressor FILES ${SOURCEFILES_UTIL_COMPRESSOR_LIST})
	source_group(SourceFiles\\util FILES ${SOURCEFILES_UTIL_LIST})
	source_group(SourceFiles\\base\\hashmap FILES ${SOURCEFILES_BASE_HASHMAP_LIST})
	source_group(SourceFiles\\base FILES ${SOURCEFILES_BASE_LIST})
	source_group(SourceFiles\\db\\odbc FILES ${SOURCEFILES_DB_ODBC_LIST})
	source_group(SourceFiles\\db FILES ${SOURCEFILES_DB_LIST})
	source_group(SourceFiles\\engine\\thread FILES ${SOURCEFILES_ENGINE_THREAD_LIST})
	source_group(SourceFiles\\engine FILES ${SOURCEFILES_ENGINE_LIST})
	source_group(SourceFiles\\file FILES ${SOURCEFILES_FILE_LIST})
	source_group(SourceFiles\\net\\packet FILES ${SOURCEFILES_NET_PACKET_LIST})
	source_group(SourceFiles\\net\\socket FILES ${SOURCEFILES_NET_SOCKET_LIST})
	source_group(SourceFiles\\net\\connection\\manager FILES ${SOURCEFILES_NET_CONNECTION_MANAGER_LIST})
	source_group(SourceFiles\\net\\connection FILES ${SOURCEFILES_NET_CONNECTION_LIST})
	source_group(SourceFiles\\net FILES ${SOURCEFILES_NET_LIST})
	source_group(SourceFiles\\performance FILES ${SOURCEFILES_PERFORMANCE_LIST})
	source_group(SourceFiles\\script\\cache FILES ${SOURCEFILES_SCRIPT_CACHE_LIST})
	source_group(SourceFiles\\script\\lua FILES ${SOURCEFILES_SCRIPT_LUA_LIST})
	source_group(SourceFiles\\script FILES ${SOURCEFILES_SCRIPT_LIST})
	source_group(SourceFiles\\sys\\memory FILES ${SOURCEFILES_SYS_MEMORY_LIST})
	source_group(SourceFiles\\sys FILES ${SOURCEFILES_SYS_LIST})
	source_group(SourceFiles\\event FILES ${SOURCEFILES_EVENT_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\util\\compressor FILES ${HEADERFILES_UTIL_COMPRESSOR_LIST})
	source_group(HeaderFiles\\util FILES ${HEADERFILES_UTIL_LIST})
	source_group(HeaderFiles\\base\\hashmap FILES ${HEADERFILES_BASE_HASHMAP_LIST})
	source_group(HeaderFiles\\base FILES ${HEADERFILES_BASE_LIST})
	source_group(HeaderFiles\\db\\odbc FILES ${HEADERFILES_DB_ODBC_LIST})
	source_group(HeaderFiles\\db FILES ${HEADERFILES_DB_LIST})
	source_group(HeaderFiles\\engine\\thread FILES ${HEADERFILES_ENGINE_THREAD_LIST})
	source_group(HeaderFiles\\engine FILES ${HEADERFILES_ENGINE_LIST})
	source_group(HeaderFiles\\file FILES ${HEADERFILES_FILE_LIST})
	source_group(HeaderFiles\\net\\socket FILES ${HEADERFILES_NET_SOCKET_LIST})
	source_group(HeaderFiles\\net\\packet FILES ${HEADERFILES_NET_PACKET_LIST})
	source_group(HeaderFiles\\net\\connection\\manager FILES ${HEADERFILES_NET_CONNECTION_MANAGER_LIST})
	source_group(HeaderFiles\\net\\connection FILES ${HEADERFILES_NET_CONNECTION_LIST})
	source_group(HeaderFiles\\net FILES ${HEADERFILES_NET_LIST})
	source_group(HeaderFiles\\performance FILES ${HEADERFILES_PERFORMANCE_LIST})
	source_group(HeaderFiles\\script\\cache FILES ${HEADERFILES_SCRIPT_CACHE_LIST})
	source_group(HeaderFiles\\script\\lua FILES ${HEADERFILES_SCRIPT_LUA_LIST})
	source_group(HeaderFiles\\script FILES ${HEADERFILES_SCRIPT_LIST})
	source_group(HeaderFiles\\sys\\memory FILES ${HEADERFILES_SYS_MEMORY_LIST})
	source_group(HeaderFiles\\sys FILES ${HEADERFILES_SYS_LIST})
	source_group(HeaderFiles\\event FILES ${HEADERFILES_EVENT_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
endif()


SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH})
ADD_LIBRARY(pf_core SHARED
	${SOURCEFILES_UTIL_COMPRESSOR_LIST}
	${SOURCEFILES_UTIL_LIST}
	${SOURCEFILES_BASE_HASHMAP_LIST}
	${SOURCEFILES_BASE_LIST}
	${SOURCEFILES_DB_ODBC_LIST}
	${SOURCEFILES_DB_LIST}
	${SOURCEFILES_ENGINE_THREAD_LIST}
	${SOURCEFILES_ENGINE_LIST}
	${SOURCEFILES_FILE_LIST}
	${SOURCEFILES_NET_PACKET_LIST}
	${SOURCEFILES_NET_SOCKET_LIST}
	${SOURCEFILES_NET_CONNECTION_MANAGER_LIST}
	${SOURCEFILES_NET_CONNECTION_LIST}
	${SOURCEFILES_NET_LIST}
	${SOURCEFILES_PERFORMANCE_LIST}
	${SOURCEFILES_SCRIPT_CACHE_LIST}
	${SOURCEFILES_SCRIPT_LUA_LIST}
	${SOURCEFILES_SCRIPT_LIST}
	${SOURCEFILES_SYS_MEMORY_LIST}
	${SOURCEFILES_SYS_LIST}
	${SOURCEFILES_EVENT_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_UTIL_COMPRESSOR_LIST}
	${HEADERFILES_UTIL_LIST}
	${HEADERFILES_BASE_HASHMAP_LIST}
	${HEADERFILES_BASE_LIST}
	${HEADERFILES_DB_ODBC_LIST}
	${HEADERFILES_DB_LIST}
	${HEADERFILES_ENGINE_THREAD_LIST}
	${HEADERFILES_ENGINE_LIST}
	${HEADERFILES_FILE_LIST}
	${HEADERFILES_NET_SOCKET_LIST}
	${HEADERFILES_NET_PACKET_LIST}
	${HEADERFILES_NET_CONNECTION_MANAGER_LIST}
	${HEADERFILES_NET_CONNECTION_LIST}
	${HEADERFILES_NET_LIST}
	${HEADERFILES_PERFORMANCE_LIST}
	${HEADERFILES_SCRIPT_CACHE_LIST}
	${HEADERFILES_SCRIPT_LUA_LIST}
	${HEADERFILES_SCRIPT_LIST}
	${HEADERFILES_SYS_MEMORY_LIST}
	${HEADERFILES_SYS_LIST}
	${HEADERFILES_EVENT_LIST}
	${HEADERFILES_LIST}
)


##update_end

if (WIN32)
TARGET_LINK_LIBRARIES(pf_core ws2_32.lib odbc32.lib odbccp32.lib lua.lib iconv.lib)
else()
  if(USE_32BITS)
    message(STATUS "using 32bits") 
    TARGET_LINK_LIBRARIES(pf_core pthread odbc iconv lua)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
  else()
    message(STATUS "using 64bits") 
    TARGET_LINK_LIBRARIES(pf_core pthread odbc iconv lua)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
  endif(USE_32BITS)
endif(WIN32)
