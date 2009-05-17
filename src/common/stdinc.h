/* Conspire
 * Copyright (C) 2009 William Pitcock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <glib.h>
#include <mowgli.h>

#ifndef __CONSPIRE_STDINC_H__GUARD
#define __CONSPIRE_STDINC_H__GUARD

#ifdef _MSC_VER
# pragma warning (disable: 4996)
#endif

#ifndef _WIN32
# include <unistd.h>
# include <sys/wait.h>
# include <pwd.h>
# include <sys/time.h>
# include <sys/utsname.h>
# include <dirent.h>
#endif

#include <signal.h>

#ifdef _WIN32

# include <process.h>

# define F_OK		00
# define W_OK		00
# define X_OK		00

# define S_IRUSR	00
# define S_IWUSR	00
# define S_IXUSR	00

# define SIGPIPE	13

/* dcc.c uses S_ISDIR(), this implements it. --nenolod */
# define __S_ISTYPE(mode, mask) (((mode) & S_IFMT) == (mask))
# define S_ISDIR(mode) __S_ISTYPE((mode), S_IFDIR)

#endif

#ifndef _WIN32
# define BIG_STR_TO_INT(x) strtoull(x, NULL, 10)
#else
# define BIG_STR_TO_INT(x) _strtoui64(x, NULL, 10)
#endif

#endif
