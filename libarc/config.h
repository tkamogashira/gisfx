/*
    Copyright (C) 2000 Masanao Izumo <mo@goice.co.jp>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef ___CONFIG_H_
#define ___CONFIG_H_


/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you want to enable pipe command scheme ("command|") */
#define PIPE_SCHEME_ENABLE

/* Define if you want to appended onty a user's home directory if a filename
 * is beginning with '~'
 */
#define TILD_SCHEME_ENABLE

/* Define if you want to use soft directory cache */
/* #define URL_DIR_CACHE_ENABLE */


/* Define if your compiler supported `volatile' declare */
#define HAS_VOLATILE


#define SUPPORT_SOCKET


#include <errno.h>
#ifdef sun
extern int errno;
#define strerror(n) (sys_errlist[n])
extern char *sys_errlist[];
#endif /* sun */

/* The path separator */
/* Windows: "\"
 * Cygwin:  both "\" and "/"
 * Macintosh: ":"
 * Unix: "/"
 */
#if defined(__W32__)
#  define PATH_SEP '\\'
#  define PATH_STRING "\\"
#if defined(__CYGWIN32__) || defined(__MINGW32__)
#  define PATH_SEP2 '/'
#endif
#elif defined(__MACOS__)
#  define PATH_SEP ':'
#  define PATH_STRING ":"
#else
#  define PATH_SEP '/'
#  define PATH_STRING "/"
#endif

#ifdef PATH_SEP2
#define IS_PATH_SEP(c) ((c) == PATH_SEP || (c) == PATH_SEP2)
#else
#define IS_PATH_SEP(c) ((c) == PATH_SEP)
#endif


#ifdef HAS_VOLATILE
#define VOLATILE_TOUCH(val) /* do nothing */
#define VOLATILE volatile
#else
extern int url_volatile_touch(void* dmy);
#define VOLATILE_TOUCH(val) url_volatile_touch(&(val))
#define VOLATILE
#endif /* HAS_VOLATILE */

/* DEC has 64 bit long words */
#if defined(__osf__)
typedef unsigned int uint32;
typedef int int32;
#else
typedef unsigned long uint32;
typedef long int32;
#endif
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned char uint8;
typedef signed char int8;

#ifdef __BORLANDC__
/* strncasecmp()-> strncmpi(char *,char *,size_t) */
#define strncasecmp(a,b,c) strncmpi(a,b,c)
#endif /* __BORLANDC__ */

#endif /* ___CONFIG_H_ */
