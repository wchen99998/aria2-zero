cmake_minimum_required(VERSION 3.20)

project(aria2-zero LANGUAGES C CXX VERSION 1.37.0)

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ARIA2_ENABLE_NLS "Build translations with msgfmt" ON)
option(ARIA2_USE_LIBUV "Use libuv poll backend" OFF)
option(ARIA2_WITH_BREAKPAD "Enable breakpad crash dumps" OFF)
set(ARIA2_SSL_EXTERNAL_DEFAULT ON)
if(WIN32 OR APPLE)
  set(ARIA2_SSL_EXTERNAL_DEFAULT OFF)
endif()
option(ARIA2_SSL_EXTERNAL "Use OpenSSL TLS implementation" ${ARIA2_SSL_EXTERNAL_DEFAULT})

function(_sanitize_header_macro input output)
  string(REPLACE "/" "_" _name "${input}")
  string(REPLACE "." "_" _name "${_name}")
  string(REPLACE "-" "_" _name "${_name}")
  string(TOUPPER "${_name}" _name)
  set(${output} "HAVE_${_name}" PARENT_SCOPE)
endfunction()

function(check_header_macro header)
  _sanitize_header_macro("${header}" macro)
  check_include_file("${header}" ${macro})
  set(${macro} ${${macro}} PARENT_SCOPE)
endfunction()

function(select_first_target out)
  foreach(candidate IN LISTS ARGN)
    if(TARGET "${candidate}")
      set(${out} "${candidate}" PARENT_SCOPE)
      return()
    endif()
  endforeach()
  set(${out} "" PARENT_SCOPE)
endfunction()

set(PACKAGE "aria2-zero")
set(PACKAGE_NAME "${PACKAGE}")
set(PACKAGE_VERSION "${PROJECT_VERSION}-development")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE_NAME}")
set(PACKAGE_URL "https://aria2.github.io")
set(PACKAGE_BUGREPORT "https://github.com/zeromake/aria2-zero/issues")
set(VERSION "${PACKAGE_VERSION}")
set(HOST "${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}")
set(BUILD "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")
set(TARGET "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")

set(common_headers
  "argz.h"
  "arpa/inet.h"
  "fcntl.h"
  "float.h"
  "inttypes.h"
  "langinfo.h"
  "libintl.h"
  "limits.h"
  "libgen.h"
  "locale.h"
  "malloc.h"
  "math.h"
  "memory.h"
  "netdb.h"
  "netinet/in.h"
  "netinet/tcp.h"
  "poll.h"
  "port.h"
  "signal.h"
  "stddef.h"
  "stdint.h"
  "stdio.h"
  "stdio_ext.h"
  "stdlib.h"
  "string.h"
  "strings.h"
  "sys/epoll.h"
  "sys/event.h"
  "sys/ioctl.h"
  "sys/mman.h"
  "sys/param.h"
  "sys/resource.h"
  "sys/stat.h"
  "sys/signal.h"
  "sys/socket.h"
  "sys/time.h"
  "sys/utime.h"
  "sys/types.h"
  "sys/uio.h"
  "sys/utsname.h"
  "termios.h"
  "time.h"
  "unistd.h"
  "utime.h"
  "wchar.h"
  "ifaddrs.h"
  "pwd.h"
  "stdbool.h"
  "pthread.h"
  "getopt.h"
  "windows.h"
  "winsock2.h"
  "ws2tcpip.h"
  "mmsystem.h"
  "io.h"
  "iphlpapi.h"
  "winioctl.h"
  "share.h"
  "wincrypt.h"
)
foreach(header IN LISTS common_headers)
  check_header_macro("${header}")
endforeach()

check_symbol_exists(getaddrinfo "sys/types.h;sys/socket.h;netdb.h" HAVE_GETADDRINFO)
check_c_source_compiles(
  "#include <time.h>\nint main(void){struct timespec ts; (void)ts; return 0;}\n"
  HAVE_A2_STRUCT_TIMESPEC
)
check_symbol_exists(sleep "unistd.h" HAVE_SLEEP)
check_symbol_exists(usleep "unistd.h" HAVE_USLEEP)
check_symbol_exists(localtime_r "time.h" HAVE_LOCALTIME_R)
check_symbol_exists(gettimeofday "sys/time.h" HAVE_GETTIMEOFDAY)
check_symbol_exists(strptime "time.h" HAVE_STRPTIME)
check_symbol_exists(timegm "time.h" HAVE_TIMEGM)
check_symbol_exists(asctime_r "time.h" HAVE_ASCTIME_R)
check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
check_symbol_exists(basename "libgen.h" HAVE_BASENAME)
check_symbol_exists(gai_strerror "sys/types.h;sys/socket.h;netdb.h" HAVE_GAI_STRERROR)
check_symbol_exists(strcasecmp "strings.h" HAVE_STRCASECMP)
check_symbol_exists(strncasecmp "strings.h" HAVE_STRNCASECMP)
check_symbol_exists(fallocate "fcntl.h" HAVE_FALLOCATE)
check_symbol_exists(posix_fallocate "fcntl.h" HAVE_POSIX_FALLOCATE)
check_symbol_exists(gzbuffer "zlib.h" HAVE_GZBUFFER)
check_symbol_exists(gzsetparams "zlib.h" HAVE_GZSETPARAMS)
check_symbol_exists(sqlite3_open_v2 "sqlite3.h" HAVE_SQLITE3_OPEN_V2)
check_symbol_exists(X509_get_default_cert_file "openssl/x509.h" HAVE_X509_GET_DEFAULT_CERT_FILE)
check_symbol_exists(daemon "unistd.h" HAVE_DAEMON)

check_cxx_source_compiles("int main(){ __int64 v = 0; (void)v; return 0; }" HAVE___INT64)
check_cxx_source_compiles("int main(){ long long v = 0; (void)v; return 0; }" HAVE_LONG_LONG)

set(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h")
check_type_size("ssize_t" SIZEOF_SSIZE_T)
unset(CMAKE_EXTRA_INCLUDE_FILES)
if(SIZEOF_SSIZE_T)
  set(HAVE_SSIZE_T 1)
endif()

if(HAVE_FALLOCATE OR HAVE_POSIX_FALLOCATE OR WIN32 OR APPLE)
  set(HAVE_SOME_FALLOCATE 1)
endif()

set(ENABLE_METALINK 1)
set(ENABLE_XML_RPC 1)
set(ENABLE_BITTORRENT 1)
set(ENABLE_SSL 1)
set(HAVE_LIBCARES 1)
set(HAVE_LIBSSH2 1)
set(HAVE_OPENSSL 1)
set(HAVE_EVP_SHA224 1)
set(HAVE_EVP_SHA256 1)
set(HAVE_EVP_SHA384 1)
set(HAVE_EVP_SHA512 1)
set(HAVE_SQLITE3 1)
set(HAVE_LIBEXPAT 1)
set(HAVE_ZLIB 1)
set(HAVE_SOME_XMLLIB 1)
set(ENABLE_WEBSOCKET 1)
set(ENABLE_ASYNC_DNS 1)
set(USE_INTERNAL_MD 1)
set(ENABLE_COMMONAD_DELTA_DEBUG 1)
set(ENABLE_NLS 1)

if(WIN32)
  set(_POSIX_C_SOURCE 1)
  set(SCHANNEL_USE_BLACKLISTS 1)
else()
  set(_GNU_SOURCE 1)
  set(ENABLE_PTHREAD 1)
endif()

if(ARIA2_SSL_EXTERNAL)
  set(HAVE_OPENSSL 1)
else()
  if(WIN32)
    set(SECURITY_WIN32 1)
    set(HAVE_WINTLS 1)
  elseif(APPLE)
    set(HAVE_APPLETLS 1)
  endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
  if(NOT ARIA2_USE_LIBUV)
    set(HAVE_EPOLL 1)
  endif()
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "BSD")
  if(NOT ARIA2_USE_LIBUV)
    set(HAVE_KQUEUE 1)
  endif()
endif()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.cmake.in"
  "${CMAKE_BINARY_DIR}/generated/config.h"
  @ONLY
)

find_package(Threads REQUIRED)
find_package(EXPAT REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS locale)

find_package(unofficial-c-ares CONFIG QUIET)
select_first_target(CARES_TARGET
  unofficial::c-ares::cares
  unofficial-c-ares::cares
  c-ares::cares
)
if(NOT CARES_TARGET)
  find_path(CARES_INCLUDE_DIR NAMES ares.h REQUIRED)
  find_library(CARES_LIBRARY NAMES cares c-ares libcares REQUIRED)
  add_library(cares::cares UNKNOWN IMPORTED)
  set_target_properties(cares::cares PROPERTIES
    IMPORTED_LOCATION "${CARES_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CARES_INCLUDE_DIR}"
  )
  set(CARES_TARGET cares::cares)
endif()

find_package(unofficial-libssh2 CONFIG QUIET)
select_first_target(SSH2_TARGET
  unofficial::libssh2::libssh2
  unofficial-libssh2::libssh2
  libssh2::libssh2
  Libssh2::libssh2
)
if(NOT SSH2_TARGET)
  find_path(SSH2_INCLUDE_DIR NAMES libssh2.h REQUIRED)
  find_library(SSH2_LIBRARY NAMES ssh2 libssh2 REQUIRED)
  add_library(libssh2::manual UNKNOWN IMPORTED)
  set_target_properties(libssh2::manual PROPERTIES
    IMPORTED_LOCATION "${SSH2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SSH2_INCLUDE_DIR}"
  )
  set(SSH2_TARGET libssh2::manual)
endif()

set(ARIA2_SOURCE_DIRS
  "src/core"
  "src/crypto/common"
  "src/network"
  "src/parser"
  "src/parser/json"
  "src/parser/xml"
  "src/protocol"
  "src/protocol/announce"
  "src/protocol/bt"
  "src/protocol/lpd"
  "src/protocol/metalink"
  "src/protocol/peer"
  "src/protocol/piece"
  "src/protocol/utm"
  "src/protocol/ws"
  "src/rpc"
  "src/storage"
  "src/stream"
  "src/util"
)

set(ARIA2_SOURCES)
set(ARIA2_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/compat"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tls"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/crypto"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/poll"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/sftp"
)
foreach(dir IN LISTS ARIA2_SOURCE_DIRS)
  file(GLOB dir_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.cc")
  list(APPEND ARIA2_SOURCES ${dir_sources})
  list(APPEND ARIA2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
endforeach()

file(GLOB WSLAY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/deps/wslay/lib/*.c")
file(GLOB POLL_SELECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/poll/select/*.cc")
file(GLOB XML_EXPAT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/xml/expat/*.cc")
file(GLOB SFTP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/sftp/*.cc")
list(APPEND ARIA2_SOURCES
  ${WSLAY_SOURCES}
  ${POLL_SELECT_SOURCES}
  ${XML_EXPAT_SOURCES}
  ${SFTP_SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/compat/boost_locale_libintl.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/uri_split.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/compat/gai_strerror.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/compat/a2io.cc"
)

if(WIN32)
  file(GLOB WIN32_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/win32/*.cc")
  list(APPEND ARIA2_SOURCES ${WIN32_SOURCES})
endif()

if(ARIA2_SSL_EXTERNAL)
  file(GLOB TLS_LIBSSL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/tls/libssl/*.cc")
  file(GLOB CRYPTO_LIBSSL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/libssl/*.cc")
  list(APPEND ARIA2_SOURCES ${TLS_LIBSSL_SOURCES} ${CRYPTO_LIBSSL_SOURCES})
else()
  if(WIN32)
    file(GLOB TLS_WINTLS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/tls/wintls/*.cc")
    list(APPEND ARIA2_SOURCES ${TLS_WINTLS_SOURCES})
  elseif(APPLE)
    file(GLOB TLS_APPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/tls/apple/*.cc")
    list(APPEND ARIA2_SOURCES ${TLS_APPLE_SOURCES})
  endif()
  file(GLOB CRYPTO_LIBSSL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/libssl/*.cc")
  list(APPEND ARIA2_SOURCES ${CRYPTO_LIBSSL_SOURCES})
endif()

if(ARIA2_USE_LIBUV)
  find_package(unofficial-libuv CONFIG QUIET)
  select_first_target(LIBUV_TARGET
    unofficial::libuv::libuv
    unofficial-libuv::libuv
    uv::uv
  )
  if(NOT LIBUV_TARGET)
    find_path(LIBUV_INCLUDE_DIR NAMES uv.h REQUIRED)
    find_library(LIBUV_LIBRARY NAMES uv libuv REQUIRED)
    add_library(libuv::manual UNKNOWN IMPORTED)
    set_target_properties(libuv::manual PROPERTIES
      IMPORTED_LOCATION "${LIBUV_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBUV_INCLUDE_DIR}"
    )
    set(LIBUV_TARGET libuv::manual)
  endif()
  file(GLOB POLL_UV_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/poll/libuv/*.cc")
  list(APPEND ARIA2_SOURCES ${POLL_UV_SOURCES})
  set(HAVE_LIBUV 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
  file(GLOB POLL_EPOLL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/poll/epoll/*.cc")
  list(APPEND ARIA2_SOURCES ${POLL_EPOLL_SOURCES})
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "BSD")
  file(GLOB POLL_KQUEUE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/poll/kqueue/*.cc")
  list(APPEND ARIA2_SOURCES ${POLL_KQUEUE_SOURCES})
endif()

if(HAVE_POLL AND NOT ARIA2_USE_LIBUV)
  file(GLOB POLL_POLL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/poll/poll/*.cc")
  list(APPEND ARIA2_SOURCES ${POLL_POLL_SOURCES})
endif()

if(NOT HAVE_ASCTIME_R)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/asctime_r.c")
endif()
if(NOT HAVE_GETADDRINFO)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/getaddrinfo.c")
endif()
if(NOT HAVE_GETTIMEOFDAY)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/gettimeofday.c")
endif()
if(NOT HAVE_LOCALTIME_R)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/localtime_r.c")
endif()
if(NOT HAVE_STRPTIME)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/strptime.c")
endif()
if(NOT HAVE_TIMEGM)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/timegm.c")
endif()
if(NOT HAVE_STRNCASECMP)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/strncasecmp.c")
endif()
if(NOT HAVE_GETOPT_H)
  list(APPEND ARIA2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compat/_getopt.c")
endif()

add_library(aria2 ${ARIA2_SOURCES})
if(BUILD_SHARED_LIBS)
  target_compile_definitions(aria2 PRIVATE ARIA2_BUILDING_LIBRARY)
else()
  target_compile_definitions(aria2 PUBLIC ARIA2_STATICLIB)
endif()

target_compile_definitions(aria2
  PUBLIC
    HAVE_CONFIG_H=1
    CXX11_OVERRIDE=override
    WSLAY_VERSION="${PACKAGE_VERSION}"
)
if(NOT WIN32)
  target_compile_definitions(aria2 PUBLIC _GNU_SOURCE=1)
endif()

target_include_directories(aria2
  PUBLIC
    "${CMAKE_BINARY_DIR}/generated"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/wslay/lib/includes"
  PRIVATE
    ${ARIA2_INCLUDE_DIRS}
)

target_link_libraries(aria2
  PUBLIC
    Threads::Threads
    ${CARES_TARGET}
    ${SSH2_TARGET}
    Boost::locale
  PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(TARGET EXPAT::EXPAT)
  target_link_libraries(aria2 PUBLIC EXPAT::EXPAT)
elseif(TARGET expat::expat)
  target_link_libraries(aria2 PUBLIC expat::expat)
endif()

if(TARGET SQLite::SQLite3)
  target_link_libraries(aria2 PUBLIC SQLite::SQLite3)
elseif(TARGET unofficial::sqlite3::sqlite3)
  target_link_libraries(aria2 PUBLIC unofficial::sqlite3::sqlite3)
elseif(TARGET unofficial-sqlite3::sqlite3)
  target_link_libraries(aria2 PUBLIC unofficial-sqlite3::sqlite3)
endif()

target_link_libraries(aria2 PUBLIC ZLIB::ZLIB)

if(ARIA2_USE_LIBUV)
  target_link_libraries(aria2 PRIVATE ${LIBUV_TARGET})
endif()

if(WIN32)
  target_link_libraries(aria2 PRIVATE ws2_32 shell32 iphlpapi crypt32 secur32)
elseif(APPLE)
  find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
  find_library(SECURITY_FRAMEWORK Security)
  if(COREFOUNDATION_FRAMEWORK)
    target_link_libraries(aria2 PRIVATE "${COREFOUNDATION_FRAMEWORK}")
  endif()
  if(SECURITY_FRAMEWORK)
    target_link_libraries(aria2 PRIVATE "${SECURITY_FRAMEWORK}")
  endif()
endif()

add_executable(aria2c "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")
target_compile_definitions(aria2c PRIVATE HAVE_CONFIG_H=1)
target_include_directories(aria2c PRIVATE
  "${CMAKE_BINARY_DIR}/generated"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/compat"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tls"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/network"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/storage"
)
target_link_libraries(aria2c PRIVATE aria2)

if(ARIA2_WITH_BREAKPAD)
  find_package(unofficial-breakpad CONFIG QUIET)
  select_first_target(BREAKPAD_TARGET
    unofficial::breakpad::breakpad_client
    unofficial-breakpad::breakpad_client
  )
  if(BREAKPAD_TARGET)
    target_compile_definitions(aria2c PRIVATE ENABLE_BREAKPAD=1)
    target_link_libraries(aria2c PRIVATE ${BREAKPAD_TARGET})
  else()
    message(FATAL_ERROR "ARIA2_WITH_BREAKPAD is ON, but breakpad package was not found.")
  endif()
endif()

if(ARIA2_ENABLE_NLS)
  set(_msgfmt_hints "")
  if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_HOST_TRIPLET)
    list(APPEND _msgfmt_hints "${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/tools/gettext/bin")
  endif()
  if(DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    get_filename_component(_vcpkg_root "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
    get_filename_component(_vcpkg_root "${_vcpkg_root}" DIRECTORY)
    if(DEFINED VCPKG_HOST_TRIPLET)
      list(APPEND _msgfmt_hints "${_vcpkg_root}/installed/${VCPKG_HOST_TRIPLET}/tools/gettext/bin")
    endif()
  endif()
  find_program(MSGFMT_EXECUTABLE NAMES msgfmt HINTS ${_msgfmt_hints})
  if(NOT MSGFMT_EXECUTABLE)
    message(FATAL_ERROR "msgfmt not found. Install gettext[tools] via vcpkg for host triplet.")
  endif()

  file(GLOB PO_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/*.po")
  set(MO_OUTPUTS "")
  foreach(po_file IN LISTS PO_FILES)
    get_filename_component(lang "${po_file}" NAME_WE)
    set(mo_file "${CMAKE_BINARY_DIR}/locale/${lang}/LC_MESSAGES/aria2-zero.mo")
    add_custom_command(
      OUTPUT "${mo_file}"
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/locale/${lang}/LC_MESSAGES"
      COMMAND "${MSGFMT_EXECUTABLE}" -o "${mo_file}" "${po_file}"
      DEPENDS "${po_file}"
      COMMENT "Generating ${mo_file}"
      VERBATIM
    )
    list(APPEND MO_OUTPUTS "${mo_file}")
  endforeach()
  add_custom_target(aria2_translations ALL DEPENDS ${MO_OUTPUTS})
  add_dependencies(aria2c aria2_translations)
endif()
