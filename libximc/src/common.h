#ifndef INC_COMMON_H
#define INC_COMMON_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* XCode build is autoconf-free and set this macro automatically */

#ifndef HAVE_CONFIG_H
/* Workaround for a non-autoconf system (XCode, vcproj)
 * All HAVE_* flags must be defined/undefined here
 * +locks -java -gigo
 */
#ifdef HAVE_LOCKS
#error Wrong assumption
#endif
#define HAVE_LOCKS

#endif

#ifdef _MSC_VER

#if !defined(__cplusplus)
typedef unsigned char bool;
#define true 1
#define false 0
#endif

#else

#include <strings.h>
#include <stdint.h>
#include <stdbool.h>

#endif

#ifdef __BIG_ENDIAN__
#error Big-endian is not supported
#endif

typedef unsigned char byte;

#if defined(_WIN32)

#ifndef _WINDOWS
#define _WINDOWS
#endif

#ifndef WINVER
#define WINVER 0x0500          // windows 2000
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500     // windows 2000
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <windows.h>

#endif

#ifdef _WIN32
typedef HANDLE handle_t;
#else
typedef int handle_t;
#endif

#define XIMC_UNUSED(x) (void)x;

// common headers
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#ifdef _MSC_VER

#include <io.h>
#include <fcntl.h>
#define portable_strdup _strdup
#define portable_snprintf _snprintf
#define portable_strncasecmp _strnicmp
#define portable_strcasecmp _stricmp

typedef SSIZE_T ssize_t;

#else

#define portable_strdup strdup
#define portable_snprintf snprintf
#define portable_strncasecmp strncasecmp
#define portable_strcasecmp strcasecmp

#endif

#endif
