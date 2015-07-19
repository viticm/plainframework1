# This file generate by plain framework admin tool, date: 2015-05-11 19:16:38
SET(LIBRARY_OUTPUT_PATH ./../dll/)

ADD_DEFINITIONS(-DPF_CORE)
SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./"
  "./../../dependencies/dll/"
)

INCLUDE_DIRECTORIES(
  "../include/"
  "../../dependencies/lua/5.3.0/include/"
  "../../dependencies/iconv/include/"
  "/usr/local/unixODBC/include/"
)


SET (INCLUDE_PF_BASE_LIST
  ../include/pf/base/atomic.h
  ../include/pf/base/config.h
  ../include/pf/base/define.h
  ../include/pf/base/global.h
  ../include/pf/base/io.h
  ../include/pf/base/log.h
  ../include/pf/base/log.tpp
  ../include/pf/base/md5.h
  ../include/pf/base/singleton.h
  ../include/pf/base/string.h
  ../include/pf/base/time_manager.h
  ../include/pf/base/tinytimer.h
  ../include/pf/base/util.h
)

SET (INCLUDE_PF_BASE_HASHMAP_LIST
  ../include/pf/base/hashmap/config.h
  ../include/pf/base/hashmap/integer.h
  ../include/pf/base/hashmap/template.h
  ../include/pf/base/hashmap/template.tpp
)

SET (INCLUDE_PF_CACHE_LIST
  ../include/pf/cache/config.h
  ../include/pf/cache/dbstore.h
  ../include/pf/cache/define.h
  ../include/pf/cache/manager.h
  ../include/pf/cache/repository.h
  ../include/pf/cache/storeinterface.h
)

SET (INCLUDE_PF_DB_LIST
  ../include/pf/db/config.h
  ../include/pf/db/define.h
  ../include/pf/db/manager.h
  ../include/pf/db/query.h
)

SET (INCLUDE_PF_DB_ODBC_LIST
  ../include/pf/db/odbc/config.h
  ../include/pf/db/odbc/interface.h
  ../include/pf/db/odbc/none.h
  ../include/pf/db/odbc/system.h
)

SET (INCLUDE_PF_ENGINE_LIST
  ../include/pf/engine/config.h
  ../include/pf/engine/kernel.h
)

SET (INCLUDE_PF_ENGINE_THREAD_LIST
  ../include/pf/engine/thread/config.h
  ../include/pf/engine/thread/db.h
  ../include/pf/engine/thread/net.h
  ../include/pf/engine/thread/performance.h
  ../include/pf/engine/thread/script.h
)

SET (INCLUDE_PF_EVENT_LIST
  ../include/pf/event/config.h
  ../include/pf/event/system.h
)

SET (INCLUDE_PF_FILE_LIST
  ../include/pf/file/api.h
  ../include/pf/file/config.h
  ../include/pf/file/database.h
  ../include/pf/file/ini.h
)

SET (INCLUDE_PF_NET_LIST
  ../include/pf/net/config.h
  ../include/pf/net/manager.h
)

SET (INCLUDE_PF_NET_CONNECTION_LIST
  ../include/pf/net/connection/base.h
  ../include/pf/net/connection/config.h
  ../include/pf/net/connection/pool.h
)

SET (INCLUDE_PF_NET_CONNECTION_MANAGER_LIST
  ../include/pf/net/connection/manager/base.h
  ../include/pf/net/connection/manager/config.h
  ../include/pf/net/connection/manager/epoll.h
  ../include/pf/net/connection/manager/iocp.h
  ../include/pf/net/connection/manager/select.h
)

SET (INCLUDE_PF_NET_PACKET_LIST
  ../include/pf/net/packet/base.h
  ../include/pf/net/packet/config.h
  ../include/pf/net/packet/dynamic.h
  ../include/pf/net/packet/factory.h
  ../include/pf/net/packet/factorymanager.h
)

SET (INCLUDE_PF_NET_SOCKET_LIST
  ../include/pf/net/socket/api.h
  ../include/pf/net/socket/base.h
  ../include/pf/net/socket/compressor.h
  ../include/pf/net/socket/config.h
  ../include/pf/net/socket/encryptor.h
  ../include/pf/net/socket/extend.inl
  ../include/pf/net/socket/inputstream.h
  ../include/pf/net/socket/outputstream.h
  ../include/pf/net/socket/server.h
  ../include/pf/net/socket/stream.h
)

SET (INCLUDE_PF_PERFORMANCE_LIST
  ../include/pf/performance/config.h
  ../include/pf/performance/eyes.h
)

SET (INCLUDE_PF_SCRIPT_LIST
  ../include/pf/script/config.h
)

SET (INCLUDE_PF_SCRIPT_CACHE_LIST
  ../include/pf/script/cache/base.h
  ../include/pf/script/cache/config.h
  ../include/pf/script/cache/manager.h
)

SET (INCLUDE_PF_SCRIPT_LUA_LIST
  ../include/pf/script/lua/config.h
  ../include/pf/script/lua/filebridge.h
  ../include/pf/script/lua/interface.h
  ../include/pf/script/lua/lunar.h
  ../include/pf/script/lua/none.h
  ../include/pf/script/lua/stack.h
  ../include/pf/script/lua/system.h
  ../include/pf/script/lua/vm.h
)

SET (INCLUDE_PF_SYS_LIST
  ../include/pf/sys/assert.h
  ../include/pf/sys/config.h
  ../include/pf/sys/info.h
  ../include/pf/sys/minidump.h
  ../include/pf/sys/process.h
  ../include/pf/sys/thread.h
  ../include/pf/sys/util.h
)

SET (INCLUDE_PF_SYS_MEMORY_LIST
  ../include/pf/sys/memory/config.h
  ../include/pf/sys/memory/dynamic_allocator.h
  ../include/pf/sys/memory/share.h
  ../include/pf/sys/memory/share.tpp
  ../include/pf/sys/memory/sharemap.h
  ../include/pf/sys/memory/static_allocator.h
)

SET (INCLUDE_PF_UTIL_LIST
  ../include/pf/util/bitflag.h
  ../include/pf/util/config.h
  ../include/pf/util/random.h
)

SET (INCLUDE_PF_UTIL_COMPRESSOR_LIST
  ../include/pf/util/compressor/assistant.h
  ../include/pf/util/compressor/config.h
  ../include/pf/util/compressor/lzoconf.h
  ../include/pf/util/compressor/lzodefs.h
  ../include/pf/util/compressor/mini.h
  ../include/pf/util/compressor/minilzo.h
  ../include/pf/util/compressor/minimanager.h
)

SET (SRC_BASE_LIST
  ../src/base/define.cc
  ../src/base/global.cc
  ../src/base/io.cc
  ../src/base/log.cc
  ../src/base/md5.cc
  ../src/base/string.cc
  ../src/base/time_manager.cc
  ../src/base/tinytimer.cc
  ../src/base/util.cc
)

SET (SRC_BASE_HASHMAP_LIST
  ../src/base/hashmap/integer.cc
)

SET (SRC_CACHE_LIST
  ../src/cache/dbstore.cc
  ../src/cache/define.cc
  ../src/cache/manager.cc
  ../src/cache/repository.cc
)

SET (SRC_DB_LIST
  ../src/db/define.cc
  ../src/db/manager.cc
  ../src/db/query.cc
)

SET (SRC_DB_ODBC_LIST
  ../src/db/odbc/interface.cc
  ../src/db/odbc/system.cc
)

SET (SRC_ENGINE_LIST
  ../src/engine/kernel.cc
)

SET (SRC_ENGINE_THREAD_LIST
  ../src/engine/thread/db.cc
  ../src/engine/thread/net.cc
  ../src/engine/thread/performance.cc
  ../src/engine/thread/script.cc
)

SET (SRC_EVENT_LIST
  ../src/event/system.cc
)

SET (SRC_FILE_LIST
  ../src/file/api.cc
  ../src/file/database.cc
  ../src/file/ini.cc
)

SET (SRC_NET_LIST
  ../src/net/manager.cc
)

SET (SRC_NET_CONNECTION_LIST
  ../src/net/connection/base.cc
  ../src/net/connection/pool.cc
)

SET (SRC_NET_CONNECTION_MANAGER_LIST
  ../src/net/connection/manager/base.cc
  ../src/net/connection/manager/epoll.cc
  ../src/net/connection/manager/iocp.cc
  ../src/net/connection/manager/select.cc
)

SET (SRC_NET_PACKET_LIST
  ../src/net/packet/base.cc
  ../src/net/packet/dynamic.cc
  ../src/net/packet/factorymanager.cc
)

SET (SRC_NET_SOCKET_LIST
  ../src/net/socket/api.cc
  ../src/net/socket/base.cc
  ../src/net/socket/compressor.cc
  ../src/net/socket/encryptor.cc
  ../src/net/socket/inputstream.cc
  ../src/net/socket/outputstream.cc
  ../src/net/socket/server.cc
  ../src/net/socket/stream.cc
)

SET (SRC_PERFORMANCE_LIST
  ../src/performance/eyes.cc
)

SET (SRC_SCRIPT_CACHE_LIST
  ../src/script/cache/base.cc
  ../src/script/cache/manager.cc
)

SET (SRC_SCRIPT_LUA_LIST
  ../src/script/lua/filebridge.cc
  ../src/script/lua/interface.cc
  ../src/script/lua/lunar.cc
  ../src/script/lua/stack.cc
  ../src/script/lua/system.cc
  ../src/script/lua/vm.cc
)

SET (SRC_SYS_LIST
  ../src/sys/assert.cc
  ../src/sys/info.cc
  ../src/sys/minidump.cc
  ../src/sys/process.cc
  ../src/sys/thread.cc
  ../src/sys/util.cc
)

SET (SRC_SYS_MEMORY_LIST
  ../src/sys/memory/dynamic_allocator.cc
  ../src/sys/memory/share.cc
  ../src/sys/memory/sharemap.cc
  ../src/sys/memory/static_allocator.cc
)

SET (SRC_UTIL_LIST
  ../src/util/bitflag.cc
  ../src/util/random.cc
)

SET (SRC_UTIL_COMPRESSOR_LIST
  ../src/util/compressor/assistant.cc
  ../src/util/compressor/mini.cc
  ../src/util/compressor/minilzo.c
  ../src/util/compressor/minimanager.cc
)


ADD_LIBRARY(pf_core SHARED
  ${SRC_BASE_LIST}
  ${SRC_BASE_HASHMAP_LIST}
  ${SRC_CACHE_LIST}
  ${SRC_DB_LIST}
  ${SRC_DB_ODBC_LIST}
  ${SRC_ENGINE_LIST}
  ${SRC_ENGINE_THREAD_LIST}
  ${SRC_EVENT_LIST}
  ${SRC_FILE_LIST}
  ${SRC_NET_LIST}
  ${SRC_NET_CONNECTION_LIST}
  ${SRC_NET_CONNECTION_MANAGER_LIST}
  ${SRC_NET_PACKET_LIST}
  ${SRC_NET_SOCKET_LIST}
  ${SRC_PERFORMANCE_LIST}
  ${SRC_SCRIPT_CACHE_LIST}
  ${SRC_SCRIPT_LUA_LIST}
  ${SRC_SYS_LIST}
  ${SRC_SYS_MEMORY_LIST}
  ${SRC_UTIL_LIST}
  ${SRC_UTIL_COMPRESSOR_LIST}
)

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(pf_core ws2_32.lib lua.lib odbc32.lib odbccp32.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(pf_core lua odbc iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
